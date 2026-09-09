#include "Game.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

Game::Game()
    : window(
          sf::VideoMode({1280, 720}),
          "VectorStrike"
      ),
            enemyGrid(80.0f),
      state(GameState::Playing),
      score(0),
      health(100),
      spawnTimer(0.0f),
      collisionChecks(0),
      benchmarkMode(false),
      benchmarkEntityCount(0),
      scoreText(font),
      healthText(font),
      gameOverText(font),
      performanceText(font)
{
    window.setFramerateLimit(144);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::filesystem::path fontPath =
    std::filesystem::current_path() / "assets" / "arial.ttf";

    std::cout << "Working directory: "
            << std::filesystem::current_path()
            << '\n';

    std::cout << "Loading font: "
            << fontPath
            << '\n';

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cerr << "ERROR: Could not load Windows Arial font!\n";
    }
    else
    {
        std::cout << "Font loaded successfully!\n";
    }

    scoreText.setCharacterSize(28);
    scoreText.setPosition({20.0f, 15.0f});

    healthText.setCharacterSize(28);
    healthText.setPosition({20.0f, 45.0f});

    performanceText.setCharacterSize(22);
    performanceText.setPosition({20.0f, 80.0f});

    gameOverText.setCharacterSize(48);
    gameOverText.setPosition({450.0f, 300.0f});

    for (int i = 0; i < 5; ++i)
    {
        spawnEnemy();
    }
}

void Game::run()
{
    while (window.isOpen())
    {
        const float deltaTime = clock.restart().asSeconds();

        processEvents();

        if (state == GameState::Playing)
        {
            update(deltaTime);
        }

        render();
    }
}

void Game::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* keyPressed =
                event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::F1)
            {
                stopBenchmark();
            }

            if (keyPressed->code == sf::Keyboard::Key::F2)
            {
                startBenchmark(200);
            }

            if (keyPressed->code == sf::Keyboard::Key::F3)
            {
                startBenchmark(500);
            }

            if (keyPressed->code == sf::Keyboard::Key::F4)
            {
                startBenchmark(1000);
            }
            if (keyPressed->code == sf::Keyboard::Key::R &&
                state == GameState::GameOver)
            {
                player = Player();
                enemies.clear();
                bullets.clear();

                score = 0;
                health = 100;
                spawnTimer = 0.0f;
                state = GameState::Playing;

                for (int i = 0; i < 5; ++i)
                {
                    spawnEnemy();
                }
            }
        }

        if (const auto* mousePressed =
                event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left &&
                state == GameState::Playing &&
                !benchmarkMode)
            {
                player.shoot(bullets);
            }
        }
    }
}

void Game::update(float deltaTime)
{
    collisionChecks = 0;
    player.update(deltaTime);

    for (auto& bullet : bullets)
    {
        bullet.update(deltaTime);
    }

    if (!benchmarkMode)
    {
        for (auto& enemy : enemies)
        {
            enemy.update(deltaTime, player.getPosition());
        }
    }

    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](const Bullet& bullet)
            {
                return bullet.isOffScreen();
            }),
        bullets.end()
    );

    spawnTimer += deltaTime;

    if (!benchmarkMode && spawnTimer >= 1.0f)
    {
        spawnEnemy();
        spawnTimer = 0.0f;
    }

    handleCollisions();

    if (health <= 0)
    {
        health = 0;
        state = GameState::GameOver;
    }

    scoreText.setString(
        "Score: " + std::to_string(score)
    );

    healthText.setString(
        "Health: " + std::to_string(health)
    );

    updatePerformanceStats(deltaTime);
}

void Game::updatePerformanceStats(float deltaTime)
{
    float fps = 0.0f;

    if (deltaTime > 0.000001f)
    {
        fps = 1.0f / deltaTime;
    }

    float frameTimeMs = deltaTime * 1000.0f;

    std::ostringstream stats;

    stats << std::fixed << std::setprecision(1);

    stats << "FPS: " << fps
        << " | Frame: " << frameTimeMs << " ms"
        << " | Enemies: " << enemies.size()
        << " | Bullets: " << bullets.size()
        << " | Collision Checks: " << collisionChecks;

    if (benchmarkMode)
    {
        stats << "\nBENCHMARK: "
            << benchmarkEntityCount
            << " entities | F1=Normal F2=200 F3=500 F4=1000";
    }

    performanceText.setString(stats.str());
}

void Game::render()
{
    window.clear(sf::Color(20, 20, 30));

    player.draw(window);

    for (const auto& bullet : bullets)
    {
        bullet.draw(window);
    }

    for (const auto& enemy : enemies)
    {
        enemy.draw(window);
    }

    window.draw(scoreText);
    window.draw(healthText);
    window.draw(performanceText);

    if (state == GameState::GameOver)
    {
        gameOverText.setString(
            "GAME OVER\nPress R to restart"
        );

        window.draw(gameOverText);
    }

    window.display();
}

void Game::spawnEnemy()
{
    const int side = std::rand() % 4;

    float x = 0.0f;
    float y = 0.0f;

    switch (side)
    {
        case 0:
            x = static_cast<float>(std::rand() % 1280);
            y = 20.0f;
            break;

        case 1:
            x = static_cast<float>(std::rand() % 1280);
            y = 700.0f;
            break;

        case 2:
            x = 20.0f;
            y = static_cast<float>(std::rand() % 720);
            break;

        case 3:
            x = 1260.0f;
            y = static_cast<float>(std::rand() % 720);
            break;
    }

    enemies.emplace_back(sf::Vector2f{x, y});
}

void Game::buildSpatialHash()
{
    enemyGrid.clear();

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        enemyGrid.insert(
            static_cast<int>(i),
            enemies[i].getBounds()
        );
    }
}

void Game::handleCollisions()
{
    collisionChecks = 0;

    for (auto bulletIt = bullets.begin();
         bulletIt != bullets.end();)
    {
        bool bulletRemoved = false;

        if (!benchmarkMode)
        {
            for (auto enemyIt = enemies.begin();
                 enemyIt != enemies.end();)
            {
                ++collisionChecks;
                if (bulletIt->getBounds().findIntersection(
                        enemyIt->getBounds()))
                {
                    enemyIt = enemies.erase(enemyIt);
                    bulletIt = bullets.erase(bulletIt);

                    ++score;
                    bulletRemoved = true;

                    break;
                }
                else
                {
                    ++enemyIt;
                }
            }
        }

        if (!bulletRemoved)
        {
            ++bulletIt;
        }
    }

    if (benchmarkMode)
    {
        buildSpatialHash();

        for (std::size_t i = 0; i < enemies.size(); ++i)
        {
            sf::FloatRect bounds = enemies[i].getBounds();
            std::vector<int> candidates = enemyGrid.query(bounds);

            for (int j : candidates)
            {
                if (j <= static_cast<int>(i))
                    continue;

                ++collisionChecks;

                if (enemies[i].getBounds().findIntersection(
                        enemies[j].getBounds()))
                {
                    // Collision detected.
                    // No response yet; this is a benchmark.
                }
            }
        }
    }

    for (auto enemyIt = enemies.begin();
         enemyIt != enemies.end();)
    {
        if (enemyIt->getBounds().findIntersection(
                player.getBounds()))
        {
            enemyIt = enemies.erase(enemyIt);
            health -= 10;
        }
        else
        {
            ++enemyIt;
        }
    }
}
void Game::startBenchmark(int entityCount)
{
    benchmarkMode = true;
    benchmarkEntityCount = entityCount;

    state = GameState::Playing;

    health = 1000000;
    score = 0;

    bullets.clear();
    enemies.clear();

    spawnTimer = 0.0f;

    for (int i = 0; i < entityCount; ++i)
    {
        float x = 50.0f +
                  static_cast<float>(std::rand() % 1180);

        float y = 150.0f +
                  static_cast<float>(std::rand() % 520);

        enemies.emplace_back(sf::Vector2f{x, y});
    }
}
void Game::stopBenchmark()
{
    benchmarkMode = false;
    benchmarkEntityCount = 0;

    enemies.clear();
    bullets.clear();

    score = 0;
    health = 100;

    spawnTimer = 0.0f;

    state = GameState::Playing;

    for (int i = 0; i < 5; ++i)
    {
        spawnEnemy();
    }
}