#include "Enemy.h"
#include "AssetManager.h"
#include <cmath>
#include <cstdlib> 
#include <ctime>

// 🛠️ 終極修正：利用靜態輔助函式，在初始化清單中直接完成 SFML 3 嚴格要求的貼圖繫結！
Enemy::Enemy(sf::RenderWindow& window)
    : sprite([]() -> const sf::Texture& {
    // 現場安全提取魔王貼圖（雙路徑安全防禦）
    sf::Texture& tex = AssetManager::getInstance().getTexture("assets/boss.png");
    if (tex.getSize().x == 0) {
        tex = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/assets/boss.png");
    }
    return tex;
        }()),
    hp(500.f), maxHp(500.f), speed(150.f), moveTimer(0.f), moveInterval(2.f),
    shootTimer(0.f), shootInterval(1.2f), pattern(0), patternTimer(0.f), patternInterval(5.f),
    velocity(0.f, 0.f)
{
    // 貼圖已經在上方初始化清單中完美繫結，這裡只要直接設定 Origin 與縮放即可！
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });

    // 將魔王等比例縮放並配置在畫面上方中央
    sprite.setScale({ 0.6f, 0.6f });
    sprite.setPosition({ static_cast<float>(window.getSize().x) / 2.f, 200.f });

    // 設定隨機亂數種子
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }
}

Enemy::~Enemy() {
    for (auto* b : enemyBullets) {
        if (b != nullptr) delete b;
    }
    enemyBullets.clear();
}

void Enemy::update(float deltaTime, sf::Vector2f heroPos, bool isTimeDilated, const sf::Vector2u& windowSize) {
    // 時空斷裂連動：減速時動作與射擊速度變為 40%
    float actualDeltaTime = isTimeDilated ? deltaTime * 0.4f : deltaTime;

    moveTimer += actualDeltaTime;
    shootTimer += actualDeltaTime;
    patternTimer += actualDeltaTime;

    if (patternTimer >= patternInterval) {
        patternTimer = 0.f;
        pattern = rand() % 2;
    }

    if (moveTimer >= moveInterval) {
        moveTimer = 0.f;
        velocity.x = static_cast<float>((rand() % 3 - 1)) * speed;
        velocity.y = static_cast<float>((rand() % 3 - 1)) * speed;
    }

    sprite.move(velocity * actualDeltaTime);

    sf::Vector2f pos = sprite.getPosition();
    if (pos.x < 200.f) { pos.x = 200.f; velocity.x = -velocity.x; }
    if (pos.x > windowSize.x - 200.f) { pos.x = windowSize.x - 200.f; velocity.x = -velocity.x; }
    if (pos.y < 100.f) { pos.y = 100.f; velocity.y = -velocity.y; }
    if (pos.y > 400.f) { pos.y = 400.f; velocity.y = -velocity.y; }
    sprite.setPosition(pos);

    // 360度全面動態「隨機撲克牌花色」彈幕系統
    if (shootTimer >= shootInterval) {
        shootTimer = 0.f;

        std::string flowerPaths[4] = {
            "assets/spade.png",
            "assets/heart.png",
            "assets/diamond.png",
            "assets/club.png"
        };

        if (pattern == 0) {
            // 模式 A：放射狀環形擴散隨機四色彈幕
            int bulletCount = 18;
            for (int i = 0; i < bulletCount; ++i) {
                float angle = (i * 2 * 3.1415926f) / bulletCount;
                sf::Vector2f dir(std::cos(angle), std::sin(angle));
                float bulletSpeed = 350.f;

                int randType = rand() % 4;
                sf::Texture& randTex = AssetManager::getInstance().getTexture(flowerPaths[randType]);

                if (randTex.getSize().x == 0) {
                    randTex = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/" + flowerPaths[randType]);
                }

                enemyBullets.push_back(new Bullet(randTex, sprite.getPosition(), dir * bulletSpeed, 4, true, 10.f));

                sf::FloatRect bBounds = enemyBullets.back()->sprite.getLocalBounds();
                if (bBounds.size.x > 0) {
                    enemyBullets.back()->sprite.setScale({ 40.f / bBounds.size.x, 40.f / bBounds.size.y });
                }
            }
        }
        else {
            // 模式 B：高密度自瞄扇形隨機四色彈幕流
            sf::Vector2f bossPos = sprite.getPosition();
            sf::Vector2f baseDir = heroPos - bossPos;
            float len = std::sqrt(baseDir.x * baseDir.x + baseDir.y * baseDir.y);
            if (len != 0.f) baseDir /= len;

            float baseAngle = std::atan2(baseDir.y, baseDir.x);

            for (int i = -2; i <= 2; ++i) {
                float offsetAngle = baseAngle + (i * 0.25f);
                sf::Vector2f dir(std::cos(offsetAngle), std::sin(offsetAngle));
                float bulletSpeed = 420.f;

                int randType = rand() % 4;
                sf::Texture& randTex = AssetManager::getInstance().getTexture(flowerPaths[randType]);

                if (randTex.getSize().x == 0) {
                    randTex = AssetManager::getInstance().getTexture("C:/Users/ff091/source/repos/Project5/Project5/" + flowerPaths[randType]);
                }

                enemyBullets.push_back(new Bullet(randTex, bossPos, dir * bulletSpeed, 4, true, 10.f));

                sf::FloatRect bBounds = enemyBullets.back()->sprite.getLocalBounds();
                if (bBounds.size.x > 0) {
                    enemyBullets.back()->sprite.setScale({ 40.f / bBounds.size.x, 40.f / bBounds.size.y });
                }
            }
        }
    }

    for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
        if (*it != nullptr) {
            (*it)->update(deltaTime);

            if ((*it)->isOffScreen(windowSize)) {
                delete* it;
                it = enemyBullets.erase(it);
            }
            else {
                ++it;
            }
        }
        else {
            it = enemyBullets.erase(it);
        }
    }
}

void Enemy::takeDamage(float amount) {
    hp -= amount;
    if (hp < 0.f) hp = 0.f;
}

bool Enemy::isDead() const {
    return hp <= 0.f;
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    for (auto* b : enemyBullets) {
        if (b != nullptr) b->draw(window);
    }
}