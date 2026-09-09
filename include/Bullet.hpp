#pragma once

#include <SFML/Graphics.hpp>

class Bullet
{
public:
    Bullet(sf::Vector2f position, sf::Vector2f direction);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const;
    bool isOffScreen() const;

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    float speed;
};