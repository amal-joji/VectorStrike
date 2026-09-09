#pragma once

#include <SFML/Graphics.hpp>

class Enemy
{
public:
    enum class State
    {
        Chase,
        Attack
    };

    Enemy(
        sf::Vector2f position,
        float speed = 100.0f,
        int damage = 10
    );

    void update(
        float deltaTime,
        sf::Vector2f playerPosition,
        sf::Vector2f separation
    );

    void draw(sf::RenderWindow& window) const;

    void takeDamage(int amount);
    bool isDead() const;
    bool canAttack() const;
    void registerAttack();
    bool isInAttackRange(sf::Vector2f playerPosition) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    float getHealthRatio() const;
    bool isDamaged() const;
    int getDamage() const;
    State getState() const;

private:
    sf::CircleShape shape;

    float speed;
    int health;
    int maxHealth;
    int damage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    State state;
};