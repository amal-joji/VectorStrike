#include "Player.hpp"

#include <algorithm>
#include <cmath>

Player::Player()
    : speed(300.0f),
      shootCooldown(0.15f),
      shootTimer(0.0f)
{
    shape.setSize(sf::Vector2f{40.0f, 40.0f});

    shape.setOrigin(sf::Vector2f{20.0f, 20.0f});

    shape.setPosition(sf::Vector2f{640.0f, 360.0f});
    shape.setFillColor(sf::Color(74, 201, 255));
    shape.setOutlineColor(sf::Color(192, 243, 255));
    shape.setOutlineThickness(2.0f);
}

void Player::update(float deltaTime)
{
    sf::Vector2f direction{0.0f, 0.0f};

    // Movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        direction.y -= 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        direction.y += 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        direction.x -= 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        direction.x += 1.0f;
    }

    // Normalize diagonal movement
    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y
    );

    if (length > 0.0f)
    {
        direction /= length;
    }

    shape.move(direction * speed * deltaTime);

    // Keep player inside the window
    sf::Vector2f position = shape.getPosition();

    position.x = std::clamp(position.x, 20.0f, 1260.0f);
    position.y = std::clamp(position.y, 20.0f, 700.0f);

    shape.setPosition(position);

    // Shooting cooldown
    if (shootTimer > 0.0f)
    {
        shootTimer -= deltaTime;
    }
}

void Player::shoot(
    std::vector<Bullet>& bullets,
    sf::Vector2f target
)
{
    if (shootTimer > 0.0f)
    {
        return;
    }

    sf::Vector2f direction = target - shape.getPosition();

    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y
    );

    if (length <= 0.0f)
    {
        return;
    }

    direction /= length;

    bullets.emplace_back(
        shape.getPosition(),
        direction
    );

    shootTimer = shootCooldown;
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::Vector2f Player::getPosition() const
{
    return shape.getPosition();
}

sf::FloatRect Player::getBounds() const
{
    return shape.getGlobalBounds();
}