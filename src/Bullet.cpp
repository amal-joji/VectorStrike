#include "Bullet.hpp"

#include <cmath>

Bullet::Bullet(sf::Vector2f position, sf::Vector2f direction)
    : speed(700.0f)
{
    shape.setRadius(4.0f);
    shape.setOrigin(sf::Vector2f{4.0f, 4.0f});
    shape.setPosition(position);

    // Normalize direction
    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y
    );

    if (length > 0.0f)
    {
        direction /= length;
    }

    velocity = direction * speed;
}

void Bullet::update(float deltaTime)
{
    shape.move(velocity * deltaTime);
}

void Bullet::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::FloatRect Bullet::getBounds() const
{
    return shape.getGlobalBounds();
}

bool Bullet::isOffScreen() const
{
    sf::Vector2f position = shape.getPosition();

    return position.x < -20.0f ||
           position.x > 1300.0f ||
           position.y < -20.0f ||
           position.y > 740.0f;
}