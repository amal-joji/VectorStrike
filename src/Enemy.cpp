#include "Enemy.hpp"

#include <algorithm>
#include <cmath>

Enemy::Enemy(sf::Vector2f position, float speed, int damage)
        : speed(speed),
            health(3),
            maxHealth(3),
            damage(damage),
            attackRange(44.0f),
            attackCooldown(0.65f),
            attackTimer(0.0f),
            state(State::Chase)
{
    shape.setRadius(20.0f);

    shape.setOrigin(sf::Vector2f{20.0f, 20.0f});

    shape.setPosition(position);
    shape.setFillColor(sf::Color(236, 91, 104));
    shape.setOutlineColor(sf::Color(255, 175, 116));
    shape.setOutlineThickness(2.0f);
}

void Enemy::update(
    float deltaTime,
    sf::Vector2f playerPosition,
    sf::Vector2f separation
)
{
    sf::Vector2f direction =
        playerPosition - shape.getPosition();

    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y
    );

    state = length <= attackRange ? State::Attack : State::Chase;

    if (length > 0.0f)
    {
        direction /= length;
    }

    attackTimer = std::max(0.0f, attackTimer - deltaTime);

    if (state == State::Chase)
    {
        const sf::Vector2f movement =
            direction * speed + separation * 130.0f;
        shape.move(movement * deltaTime);
    }
}

void Enemy::draw(sf::RenderWindow& window) const
{
    window.draw(shape);

    if (!isDamaged())
        return;

    sf::RectangleShape background;
    background.setSize(sf::Vector2f{40.0f, 4.0f});
    background.setPosition(shape.getPosition() + sf::Vector2f{-20.0f, -30.0f});
    background.setFillColor(sf::Color(65, 28, 38));
    window.draw(background);

    sf::RectangleShape healthBar;
    healthBar.setSize(sf::Vector2f{40.0f * getHealthRatio(), 4.0f});
    healthBar.setPosition(shape.getPosition() + sf::Vector2f{-20.0f, -30.0f});
    healthBar.setFillColor(sf::Color(90, 224, 145));
    window.draw(healthBar);
}

void Enemy::takeDamage(int amount)
{
    health -= amount;
}

bool Enemy::isDead() const
{
    return health <= 0;
}

bool Enemy::canAttack() const
{
    return state == State::Attack && attackTimer <= 0.0f;
}

bool Enemy::isInAttackRange(sf::Vector2f playerPosition) const
{
    const sf::Vector2f difference = playerPosition - shape.getPosition();
    const float distanceSquared =
        difference.x * difference.x + difference.y * difference.y;

    return distanceSquared <= attackRange * attackRange;
}

void Enemy::registerAttack()
{
    attackTimer = attackCooldown;
}

sf::FloatRect Enemy::getBounds() const
{
    return shape.getGlobalBounds();
}

sf::Vector2f Enemy::getPosition() const
{
    return shape.getPosition();
}

float Enemy::getHealthRatio() const
{
    return static_cast<float>(std::max(health, 0)) /
           static_cast<float>(maxHealth);
}

int Enemy::getDamage() const
{
    return damage;
}

Enemy::State Enemy::getState() const
{
    return state;
}

bool Enemy::isDamaged() const
{
    return health < maxHealth;
}