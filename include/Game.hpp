#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Player.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"

class Game
{
public:
    Game();
    void run();

private:
    enum class GameState
    {
        Playing,
        GameOver
    };

    sf::RenderWindow window;
    sf::Clock clock;

    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;

    GameState state;

    int score;
    int health;

    float spawnTimer;

    sf::Font font;
    sf::Text scoreText;
    sf::Text healthText;
    sf::Text gameOverText;
    sf::Text performanceText;

    void processEvents();
    void update(float deltaTime);
    void render();

    void spawnEnemy();
    void handleCollisions();

    void updatePerformanceStats(float deltaTime);
};