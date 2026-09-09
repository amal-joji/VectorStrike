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
    struct HitEffect
    {
        sf::Vector2f position;
        float lifetime;
    };

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

    std::vector<int> candidateBuffer;
    std::vector<HitEffect> hitEffects;

    sf::Font font;

    sf::Text scoreText;
    sf::Text healthText;
    sf::Text gameOverText;
    sf::Text performanceText;
    sf::RectangleShape hudPanel;
    sf::RectangleShape playerHealthTrack;
    sf::RectangleShape playerHealthBar;
    sf::RectangleShape gameOverPanel;

    void processEvents();
    void update(float deltaTime);
    void render();

    void spawnEnemy();
    void buildSpatialHash();
    void handleCollisions();
    void updateEnemyAI(float deltaTime);
    void updateEffects(float deltaTime);
    void addHitEffect(sf::Vector2f position);
    void resetGame();
    void drawCrosshair();

    void updatePerformanceStats(float deltaTime);

    // Benchmarking
    void startBenchmark(int entityCount);
    void stopBenchmark();
};