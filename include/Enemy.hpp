#pragma once

#include <SFML/Graphics.hpp>

class Enemy
{
public:
    Enemy(sf::Vector2f position);

    void update(float deltaTime, sf::Vector2f playerPosition);

    void draw(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const;

private:
    sf::CircleShape shape;

    float speed;
};