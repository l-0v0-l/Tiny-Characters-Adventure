#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"

class Enemy {
public:
    sf::Sprite sprite;

    // 魔王的核心屬性資料成員
    float hp;
    float maxHp;
    float speed;

    // 計時器與行為控制變數
    float moveTimer;
    float moveInterval;
    float shootTimer;
    float shootInterval;

    int pattern;
    float patternTimer;
    float patternInterval;

    sf::Vector2f velocity;

    // 儲存魔王發射的所有撲克牌子彈指標
    std::vector<Bullet*> enemyBullets;

    // 建構與解構子
    Enemy(sf::RenderWindow& window);
    ~Enemy();

    // 更新與渲染接口（與 main.cpp 及 Enemy.cpp 100% 相容）
    void update(float deltaTime, sf::Vector2f heroPos, bool isTimeDilated, const sf::Vector2u& windowSize);
    void takeDamage(float amount);
    bool isDead() const;
    void draw(sf::RenderWindow& window);
};