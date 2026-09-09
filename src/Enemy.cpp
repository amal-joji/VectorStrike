#include "Enemy.hpp"

#include <cmath>

Enemy::Enemy(sf::Vector2f position)
    : speed(100.0f)
{
    shape.setRadius(20.0f);

    shape.setOrigin(sf::Vector2f{20.0f, 20.0f});

    shape.setPosition(position);
}

void Enemy::update(float deltaTime, sf::Vector2f playerPosition)
{
    sf::Vector2f direction =
        playerPosition - shape.getPosition();

    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y
    );

    // Normalize direction
    if (length > 0.0f)
    {
        direction /= length;
    }

    shape.move(direction * speed * deltaTime);
}

void Enemy::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::FloatRect Enemy::getBounds() const
{
    return shape.getGlobalBounds();
}