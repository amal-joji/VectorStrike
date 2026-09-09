#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Player.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
#include "SpatialHash.hpp"
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

    SpatialHash enemyGrid;
    GameState state;


    int score;
    int health;

    float spawnTimer;

    // Performance profiling
    long long collisionChecks;
    bool benchmarkMode;
    int benchmarkEntityCount;

    sf::Font font;

    sf::Text scoreText;
    sf::Text healthText;
    sf::Text gameOverText;
    sf::Text performanceText;

    void processEvents();
    void update(float deltaTime);
    void render();

    void spawnEnemy();
    void buildSpatialHash();
    void handleCollisions();

    void updatePerformanceStats(float deltaTime);

    // Benchmarking
    void startBenchmark(int entityCount);
    void stopBenchmark();
};