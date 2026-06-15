#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Bullet.h"
#include "AssetManager.h"

class Hero {
protected:
    float fireTimer;
    float skillCooldownTimer;
    float skillCooldown;

public:
    sf::Sprite sprite;
    float hp;
    float maxHp;
    float speed;
    float damage;
    float fireRate;
    int charType;

    bool isInvincible;
    bool isTimeDilated;
    float skillDurationTimer;
    sf::Vector2f lastMovementDir;

    Hero(const sf::Texture& texture)
        : sprite(texture), fireTimer(0.f), skillCooldownTimer(0.f),
        skillCooldown(20.f), // 🛠️ 修正：預設技能冷卻上限改為 20 秒
        hp(100.f), maxHp(100.f), speed(300.f), damage(10.f),
        fireRate(0.4f), charType(1), isInvincible(false), isTimeDilated(false),
        skillDurationTimer(0.f), lastMovementDir(0.f, -1.f) {

        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
        float scaleX = 100.f / bounds.size.x;
        float scaleY = 100.f / bounds.size.y;
        float finalScale = std::min(scaleX, scaleY);
        sprite.setScale({ finalScale, finalScale });
    }

    virtual ~Hero() = default;

    virtual void update(float deltaTime, const sf::Vector2u& windowSize) {
        fireTimer += deltaTime;

        // 更新技能持續計時器
        if (skillDurationTimer > 0.f) {
            skillDurationTimer -= deltaTime;
            if (skillDurationTimer <= 0.f) {
                isInvincible = false;
                isTimeDilated = false;
                onSkillEnd();
            }
        }

        // 更新冷卻計時器
        if (skillCooldownTimer > 0.f) {
            skillCooldownTimer -= deltaTime;
        }

        // 去彈跳去黏鍵單次 K 鍵技能觸發器
        static bool kKeyReleased = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K)) {
            if (kKeyReleased && skillCooldownTimer <= 0.f && skillDurationTimer <= 0.f) {
                useSkill();
                kKeyReleased = false;
            }
        }
        else {
            kKeyReleased = true;
        }

        sf::Vector2f movement(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movement.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movement.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement.x += 1.f;

        if (movement.x != 0.f || movement.y != 0.f) {
            float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
            movement /= length;
            lastMovementDir = movement;
            sprite.move(movement * speed * deltaTime);
        }

        sf::FloatRect bounds = sprite.getGlobalBounds();
        sf::Vector2f pos = sprite.getPosition();
        sf::Vector2f origin = sprite.getOrigin();

        float minX = 50.f;
        float maxX = static_cast<float>(windowSize.x) - 50.f;
        float minY = 50.f;
        float maxY = static_cast<float>(windowSize.y) - 50.f;

        if (pos.x < minX) pos.x = minX;
        if (pos.x > maxX) pos.x = maxX;
        if (pos.y < minY) pos.y = minY;
        if (pos.y > maxY) pos.y = maxY;

        sprite.setPosition(pos);
    }

    virtual void attack(std::vector<Bullet>& bullets) = 0;
    virtual void useSkill() = 0;
    virtual void onSkillEnd() {}

    sf::Vector2f getPosition() const { return sprite.getPosition(); }
    bool getTimeDilationActive() const { return isTimeDilated; }
    bool isDead() const { return hp <= 0.f; }

    void draw(sf::RenderWindow& window) {
        if (isInvincible && static_cast<int>(skillDurationTimer * 15.f) % 2 == 0) {
            return;
        }
        window.draw(sprite);
    }
};

// 重裝少女 (100% 免疫傷害)
class HeavyHero : public Hero {
public:
    HeavyHero(const sf::Texture& heroTex) : Hero(heroTex) {
        charType = 1; maxHp = 150.f; hp = maxHp; speed = 220.f; damage = 35.f; fireRate = 0.35f;
    }
    void attack(std::vector<Bullet>& bullets) override {
        if (fireTimer >= fireRate && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            fireTimer = 0.f;
            sf::Texture& bTex = AssetManager::getInstance().getTexture("assets/heavy_bullet.png");
            bullets.push_back(Bullet(bTex, sprite.getPosition(), { 0.f, -550.f }, 1, false, damage));
            bullets.back().sprite.setScale({ 0.5f, 0.5f });
        }
    }
    void useSkill() override {
        std::cout << "[SKILL EFFECT] Heavy absolute defense activated!" << std::endl;
        isInvincible = true;
        skillDurationTimer = 5.f;     // 持續 5 秒
        skillCooldownTimer = 20.f;    // 🛠️ 修正：冷卻改成 20 秒
    }
};

// 狙擊少女 (時間減速魔眼)
class SniperHero : public Hero {
public:
    SniperHero(const sf::Texture& heroTex) : Hero(heroTex) {
        charType = 2; maxHp = 90.f; hp = maxHp; speed = 340.f; damage = 55.f; fireRate = 0.60f;
    }
    void attack(std::vector<Bullet>& bullets) override {
        if (fireTimer >= fireRate && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            fireTimer = 0.f;
            sf::Texture& bTex = AssetManager::getInstance().getTexture("assets/bullet_laser.png");
            bullets.push_back(Bullet(bTex, sprite.getPosition(), { 0.f, -1000.f }, 2, false, damage));
            bullets.back().sprite.setScale({ 0.25f, 1.0f });
        }
    }
    void useSkill() override {
        std::cout << "[SKILL EFFECT] Sniper time dilation active!" << std::endl;
        isTimeDilated = true;
        skillDurationTimer = 4.f;     // 持續 4 秒
        skillCooldownTimer = 20.f;    // 🛠️ 修正：冷卻改成 20 秒
    }
};

// 散彈少女 (狂暴速射模式)
class ScatterHero : public Hero {
private:
    float originalFireRate;
public:
    ScatterHero(const sf::Texture& heroTex) : Hero(heroTex) {
        charType = 3; maxHp = 110.f; hp = maxHp; speed = 280.f; damage = 15.f; fireRate = 0.25f;
        originalFireRate = fireRate;
    }
    void attack(std::vector<Bullet>& bullets) override {
        if (fireTimer >= fireRate && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            fireTimer = 0.f;
            sf::Texture& bTex = AssetManager::getInstance().getTexture("assets/scatter_bullet.png");
            sf::Vector2f pos = sprite.getPosition();

            bullets.push_back(Bullet(bTex, pos, { -150.f, -450.f }, 3, false, damage));
            bullets.push_back(Bullet(bTex, pos, { 0.f, -500.f }, 3, false, damage));
            bullets.push_back(Bullet(bTex, pos, { 150.f, -450.f }, 3, false, damage));

            for (size_t i = bullets.size() - 3; i < bullets.size(); ++i) {
                sf::FloatRect bBounds = bullets[i].sprite.getLocalBounds();
                if (bBounds.size.x > 0) {
                    bullets[i].sprite.setScale({ 50.f / bBounds.size.x, 50.f / bBounds.size.y });
                }
            }
        }
    }
    void useSkill() override {
        std::cout << "[SKILL EFFECT] Scatter crazy fire mode open!" << std::endl;
        fireRate = originalFireRate * 0.4f;
        skillDurationTimer = 4.f;     // 持續 4 秒
        skillCooldownTimer = 20.f;    // 🛠️ 修正：冷卻改成 20 秒
    }
    void onSkillEnd() override {
        fireRate = originalFireRate;
    }
};