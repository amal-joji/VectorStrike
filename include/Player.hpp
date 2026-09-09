#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Bullet.hpp"

class Player
{
public:
    Player();

    void update(float deltaTime);
    void shoot(
        std::vector<Bullet>& bullets,
        sf::Vector2f target
    );

    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;

    float speed;
    float shootCooldown;
    float shootTimer;
};