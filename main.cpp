#include <SFML/Graphics.hpp> 
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include "AssetManager.h" 
#include "Bullet.h"
#include "Hero.h"
#include "Enemy.h"
#include "UpgradeManager.h"
#include "Menu.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Space Arcade");
    window.setFramerateLimit(60);

    UpgradeManager upgradeManager;
    Menu menu(window);

    Hero* hero = nullptr;
    Enemy* boss = nullptr;
    std::vector<Bullet> playerBullets;

    bool isPaused = false;
    sf::Clock clock;

    sf::Texture& cvGardenBg = AssetManager::getInstance().getTexture("assets/garden_bg.png");
    sf::Sprite gameBackground(cvGardenBg);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (menu.getCurrentState() == MenuState::StartGame) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        isPaused = !isPaused;
                    }
                }
            }

            menu.handleEvent(*event, window, upgradeManager, isPaused);
        }

        // =====================================================================
        // 遊戲核心邏輯更新
        // =====================================================================
        if (menu.getCurrentState() == MenuState::StartGame) {
            if (!isPaused) {
                if (hero == nullptr) {
                    playerBullets.clear();
                    int choice = menu.getSelectedCharacter();

                    if (choice == 1) hero = new HeavyHero(AssetManager::getInstance().getTexture("assets/heavy.png"));
                    else if (choice == 2) hero = new SniperHero(AssetManager::getInstance().getTexture("assets/sniper.png"));
                    else hero = new ScatterHero(AssetManager::getInstance().getTexture("assets/scatter.png"));

                    hero->sprite.setPosition({ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) - 150.f });
                    boss = new Enemy(window);
                }

                hero->update(deltaTime, window.getSize());
                boss->update(deltaTime, hero->getPosition(), hero->getTimeDilationActive(), window.getSize());
                hero->attack(playerBullets);

                for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                    it->update(deltaTime);
                    if (it->isOffScreen(window.getSize())) { it = playerBullets.erase(it); }
                    else { ++it; }
                }

                float bossCenterX = boss->sprite.getPosition().x;
                for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                    sf::Vector2f bPos = it->sprite.getPosition();
                    if (bPos.y < 350.f && std::abs(bPos.x - bossCenterX) < 180.f) {
                        boss->takeDamage(hero->damage);
                        it = playerBullets.erase(it);

                        if (boss->isDead()) {
                            upgradeManager.addPoints(5);
                            menu.setCurrentState(MenuState::VictoryStory);
                            delete hero; delete boss; hero = nullptr; boss = nullptr;
                            break;
                        }
                    }
                    else { ++it; }
                }

                if (boss != nullptr && hero != nullptr && !hero->isInvincible) {
                    sf::Vector2f heroPos = hero->getPosition();
                    for (auto it = boss->enemyBullets.begin(); it != boss->enemyBullets.end();) {
                        if (*it != nullptr) {
                            sf::Vector2f bulletPos = (*it)->sprite.getPosition();
                            float dist = std::sqrt(std::pow(bulletPos.x - heroPos.x, 2) + std::pow(bulletPos.y - heroPos.y, 2));
                            if (dist < 40.f) {
                                hero->hp -= 12.f;
                                delete* it;
                                it = boss->enemyBullets.erase(it);

                                if (hero->isDead()) {
                                    upgradeManager.addPoints(2);
                                    menu.setCurrentState(MenuState::GameOverScreen);
                                    delete hero; delete boss; hero = nullptr; boss = nullptr;
                                    break;
                                }
                            }
                            else { ++it; }
                        }
                        else { it = boss->enemyBullets.erase(it); }
                    }
                }
            }
        }
        else {
            if (hero != nullptr) { delete hero; hero = nullptr; }
            if (boss != nullptr) { delete boss; boss = nullptr; }
        }

        // =====================================================================
        // 畫面渲染繪製區
        // =====================================================================
        window.clear(sf::Color::Black);

        if (menu.getCurrentState() == MenuState::StartGame && hero) {
            sf::Vector2u bgSize = cvGardenBg.getSize();
            if (bgSize.x > 0 && bgSize.y > 0) {
                gameBackground.setScale({
                    static_cast<float>(window.getSize().x) / bgSize.x,
                    static_cast<float>(window.getSize().y) / bgSize.y
                    });
            }
            window.draw(gameBackground);

            hero->draw(window);
            if (boss) boss->draw(window);
            for (auto& b : playerBullets) b.draw(window);

            if (boss) {
                for (auto& b : boss->enemyBullets) {
                    if (b != nullptr) b->draw(window);
                }
            }

            // =====================================================================
            // 🛠️ 核心功能優化：帶有外框的雙血條 UI 系統
            // =====================================================================

            // 🟢 主角血條（配置在左下角）
            if (hero->maxHp > 0.f) {
                sf::Vector2f heroHpPos(60.f, 980.f); // 左下角安全位置
                float barWidth = 300.f;             // 血條總寬度
                float barHeight = 24.f;             // 血條高度

                // 1. 繪製最大血量固定暗色底框（讓玩家知道掉了多少血）
                sf::RectangleShape heroHpBg({ barWidth, barHeight });
                heroHpBg.setPosition(heroHpPos);
                heroHpBg.setFillColor(sf::Color(40, 40, 40, 200));       // 深灰色半透明底
                heroHpBg.setOutlineThickness(2.f);
                heroHpBg.setOutlineColor(sf::Color(200, 200, 200, 255)); // 銀白色精緻外框線
                window.draw(heroHpBg);

                // 2. 繪製當前剩餘血量（綠色條）
                if (hero->hp > 0.f) {
                    float currentWidth = barWidth * (hero->hp / hero->maxHp);
                    sf::RectangleShape heroHpBar({ currentWidth, barHeight });
                    heroHpBar.setPosition(heroHpPos);
                    heroHpBar.setFillColor(sf::Color(50, 220, 50)); // 鮮綠色
                    window.draw(heroHpBar);
                }
            }

            // 🔴 魔王血條（維持在上方中央）
            if (boss && boss->maxHp > 0.f) {
                sf::Vector2f bossHpPos(static_cast<float>(window.getSize().x) / 2.f - 300.f, 50.f);
                float bossBarWidth = 600.f; // 放大血條寬度，更有 Boss 戰壓迫感
                float bossBarHeight = 26.f;

                // 1. 繪製魔王最大血量暗紅底框
                sf::RectangleShape bossHpBg({ bossBarWidth, bossBarHeight });
                bossHpBg.setPosition(bossHpPos);
                bossHpBg.setFillColor(sf::Color(60, 15, 15, 200));       // 暗紅色半透明底
                bossHpBg.setOutlineThickness(2.5f);
                bossHpBg.setOutlineColor(sf::Color(255, 215, 0, 220));   // 耀眼黃金外框線
                window.draw(bossHpBg);

                // 2. 繪製魔王當前剩餘血量（鮮紅色條）
                if (boss->hp > 0.f) {
                    float currentBossWidth = bossBarWidth * (boss->hp / boss->maxHp);
                    sf::RectangleShape bossHpBar({ currentBossWidth, bossBarHeight });
                    bossHpBar.setPosition(bossHpPos);
                    bossHpBar.setFillColor(sf::Color(230, 30, 30)); // 烈紅色
                    window.draw(bossHpBar);
                }
            }
        }

        menu.draw(window, upgradeManager, isPaused);
        window.display();
    }

    if (hero != nullptr) delete hero;
    if (boss != nullptr) delete boss;
    return 0;
}