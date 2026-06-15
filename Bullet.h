#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    int type = 1;
    bool isEnemyBullet = false;
    float damage = 10.f;

    Bullet(); // 預設建構子宣告

    Bullet(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f vel, int bulletType, bool fromEnemy, float dmg);
    Bullet(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f direction, float speed, sf::Vector2f scale);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    bool isOffScreen(const sf::Vector2u& windowSize) const;
};