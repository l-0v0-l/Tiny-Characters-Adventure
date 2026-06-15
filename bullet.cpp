#include "Bullet.h"
#include "AssetManager.h"

// 預設建構子實作：安全借用 heavy_bullet.png 出生
Bullet::Bullet()
    : sprite(AssetManager::getInstance().getTexture("assets/heavy_bullet.png")),
    velocity(0.f, 0.f), type(1), isEnemyBullet(false), damage(1.f) {
}

Bullet::Bullet(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f vel, int bulletType, bool fromEnemy, float dmg)
    : sprite(texture), velocity(vel), type(bulletType), isEnemyBullet(fromEnemy), damage(dmg)
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
    sprite.setPosition(position);
}

Bullet::Bullet(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f direction, float speed, sf::Vector2f scale)
    : sprite(texture)
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
    sprite.setPosition(position);
    sprite.setScale(scale);

    velocity = direction * speed;
    type = 0;
    isEnemyBullet = true;
    damage = 12.f;
}

void Bullet::update(float deltaTime) {
    sprite.move(velocity * deltaTime);
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Bullet::isOffScreen(const sf::Vector2u& windowSize) const {
    sf::Vector2f pos = sprite.getPosition();
    return (pos.x < -100.f || pos.y < -100.f || pos.x > windowSize.x + 100.f || pos.y > windowSize.y + 100.f);
}