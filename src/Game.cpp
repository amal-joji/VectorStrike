#include "Game.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cstdint>

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
    candidateBuffer.reserve(64);
    hitEffects.reserve(64);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::filesystem::path fontPath =
    std::filesystem::current_path() / "assets" / "arial.ttf";

    std::cout << "Working directory: "
            << std::filesystem::current_path()
            << '\n';

    std::cout << "Loading font: "
            << fontPath
            << '\n';

    if (!font.openFromFile("assets/arial.ttf") &&
        !font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cerr << "ERROR: Could not load Windows Arial font!\n";
    }
    else
    {
        std::cout << "Font loaded successfully!\n";
    }

    scoreText.setCharacterSize(28);
    scoreText.setPosition({20.0f, 15.0f});
    scoreText.setFillColor(sf::Color(230, 246, 255));

    healthText.setCharacterSize(28);
    healthText.setPosition({20.0f, 45.0f});
    healthText.setFillColor(sf::Color(255, 210, 162));

    performanceText.setCharacterSize(22);
    performanceText.setPosition({20.0f, 80.0f});
    performanceText.setFillColor(sf::Color(175, 215, 228));

    gameOverText.setCharacterSize(48);
    gameOverText.setPosition({450.0f, 300.0f});
    gameOverText.setFillColor(sf::Color(255, 236, 205));

    hudPanel.setPosition({10.0f, 10.0f});
    hudPanel.setSize({500.0f, 120.0f});
    hudPanel.setFillColor(sf::Color(8, 18, 31, 210));
    hudPanel.setOutlineColor(sf::Color(62, 116, 139));
    hudPanel.setOutlineThickness(1.0f);

    playerHealthTrack.setPosition({180.0f, 58.0f});
    playerHealthTrack.setSize({260.0f, 16.0f});
    playerHealthTrack.setFillColor(sf::Color(65, 28, 38));
    playerHealthBar.setPosition({180.0f, 58.0f});
    playerHealthBar.setFillColor(sf::Color(90, 224, 145));

    gameOverPanel.setPosition({350.0f, 245.0f});
    gameOverPanel.setSize({580.0f, 230.0f});
    gameOverPanel.setFillColor(sf::Color(8, 18, 31, 235));
    gameOverPanel.setOutlineColor(sf::Color(236, 91, 104));
    gameOverPanel.setOutlineThickness(2.0f);

    window.setMouseCursorVisible(false);

    for (int i = 0; i < 10; ++i)
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
                resetGame();
            }
        }

        if (const auto* mousePressed =
                event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left &&
                state == GameState::Playing &&
                !benchmarkMode)
            {
                const sf::Vector2i mouse = sf::Mouse::getPosition(window);
                player.shoot(
                    bullets,
                    {
                        static_cast<float>(mouse.x),
                        static_cast<float>(mouse.y)
                    }
                );
            }
        }
    }
}

void Game::update(float deltaTime)
{
    deltaTime = std::min(deltaTime, 0.05f);
    collisionChecks = 0;
    player.update(deltaTime);

    if (!benchmarkMode &&
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        const sf::Vector2i mouse = sf::Mouse::getPosition(window);
        player.shoot(
            bullets,
            {
                static_cast<float>(mouse.x),
                static_cast<float>(mouse.y)
            }
        );
    }

    if (!benchmarkMode)
    {
        updateEnemyAI(deltaTime);
    }

    for (auto& bullet : bullets)
    {
        bullet.update(deltaTime);
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

    updateEffects(deltaTime);

    spawnTimer += deltaTime;

    if (!benchmarkMode && spawnTimer >= 0.8f)
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

    playerHealthBar.setSize({
        260.0f * std::clamp(static_cast<float>(health) / 100.0f, 0.0f, 1.0f),
        16.0f
    });

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
    window.clear(sf::Color(9, 18, 30));

    player.draw(window);

    for (const auto& bullet : bullets)
    {
        bullet.draw(window);
    }

    for (const auto& enemy : enemies)
    {
        enemy.draw(window);
    }

    for (const auto& effect : hitEffects)
    {
        sf::CircleShape flash;
        const float progress = effect.lifetime / 0.22f;
        flash.setRadius(8.0f + (1.0f - progress) * 18.0f);
        flash.setOrigin(flash.getGeometricCenter());
        flash.setPosition(effect.position);
        flash.setFillColor(sf::Color(
            255,
            190,
            92,
            static_cast<std::uint8_t>(progress * 180.0f)
        ));
        window.draw(flash);
    }

    window.draw(hudPanel);
    window.draw(scoreText);
    window.draw(healthText);
    window.draw(playerHealthTrack);
    window.draw(playerHealthBar);
    window.draw(performanceText);

    if (state == GameState::GameOver)
    {
        window.draw(gameOverPanel);
        gameOverText.setString(
            "GAME OVER\n\nScore: " + std::to_string(score) +
            "\nPress R to restart"
        );
        gameOverText.setPosition({430.0f, 275.0f});
        window.draw(gameOverText);
    }

    drawCrosshair();
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

void Game::updateEnemyAI(float deltaTime)
{
    buildSpatialHash();
    const sf::Vector2f playerPosition = player.getPosition();

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        const sf::FloatRect bounds = enemies[i].getBounds();
        const sf::FloatRect searchBounds(
            {bounds.position.x - 55.0f, bounds.position.y - 55.0f},
            {bounds.size.x + 110.0f, bounds.size.y + 110.0f}
        );

        enemyGrid.query(searchBounds, candidateBuffer);

        sf::Vector2f separation{0.0f, 0.0f};
        for (int candidate : candidateBuffer)
        {
            if (candidate == static_cast<int>(i))
                continue;

            const sf::Vector2f difference =
                enemies[i].getPosition() - enemies[candidate].getPosition();
            const float distanceSquared =
                difference.x * difference.x +
                difference.y * difference.y;

            if (distanceSquared > 0.01f && distanceSquared < 55.0f * 55.0f)
            {
                const float distance = std::sqrt(distanceSquared);
                separation += (difference / distance) *
                    (1.0f - distance / 55.0f);
            }
        }

        enemies[i].update(deltaTime, playerPosition, separation);
    }
}

void Game::updateEffects(float deltaTime)
{
    for (auto& effect : hitEffects)
    {
        effect.lifetime -= deltaTime;
    }

    hitEffects.erase(
        std::remove_if(
            hitEffects.begin(),
            hitEffects.end(),
            [](const HitEffect& effect)
            {
                return effect.lifetime <= 0.0f;
            }),
        hitEffects.end()
    );
}

void Game::addHitEffect(sf::Vector2f position)
{
    if (hitEffects.size() < 64)
    {
        hitEffects.push_back({position, 0.22f});
    }
}

void Game::handleCollisions()
{
    collisionChecks = 0;

    buildSpatialHash();

    for (auto bulletIt = bullets.begin();
         bulletIt != bullets.end();)
    {
        bool bulletRemoved = false;

        if (!benchmarkMode)
        {
            enemyGrid.query(bulletIt->getBounds(), candidateBuffer);

            for (int candidate : candidateBuffer)
            {
                if (candidate < 0 ||
                    candidate >= static_cast<int>(enemies.size()))
                {
                    continue;
                }

                ++collisionChecks;
                if (bulletIt->getBounds().findIntersection(
                        enemies[candidate].getBounds()))
                {
                    const sf::Vector2f hitPosition =
                        enemies[candidate].getPosition();
                    enemies[candidate].takeDamage(1);
                    bulletIt = bullets.erase(bulletIt);
                    addHitEffect(hitPosition);

                    if (enemies[candidate].isDead())
                    {
                        enemies.erase(enemies.begin() + candidate);
                        ++score;
                    }

                    bulletRemoved = true;
                    buildSpatialHash();

                    break;
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
            enemyGrid.query(bounds, candidateBuffer);

            for (int j : candidateBuffer)
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
        ++collisionChecks;
        if (enemyIt->isInAttackRange(player.getPosition()))
        {
            if (enemyIt->canAttack())
            {
                health -= enemyIt->getDamage();
                enemyIt->registerAttack();
                addHitEffect(player.getPosition());
            }

            ++enemyIt;
        }
        else
        {
            ++enemyIt;
        }
    }
}

void Game::resetGame()
{
    player = Player();
    enemies.clear();
    bullets.clear();
    hitEffects.clear();

    score = 0;
    health = 100;
    spawnTimer = 0.0f;
    benchmarkMode = false;
    benchmarkEntityCount = 0;
    state = GameState::Playing;

    for (int i = 0; i < 10; ++i)
    {
        spawnEnemy();
    }
}

void Game::drawCrosshair()
{
    const sf::Vector2i mouse = sf::Mouse::getPosition(window);
    const sf::Vector2f position{
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    };

    sf::CircleShape ring;
    ring.setRadius(9.0f);
    ring.setOrigin(ring.getGeometricCenter());
    ring.setPosition(position);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(sf::Color(230, 246, 255, 220));
    ring.setOutlineThickness(1.5f);
    window.draw(ring);

    sf::RectangleShape horizontal;
    horizontal.setSize({24.0f, 1.0f});
    horizontal.setPosition(position + sf::Vector2f{-12.0f, -0.5f});
    horizontal.setFillColor(sf::Color(230, 246, 255, 220));
    window.draw(horizontal);

    sf::RectangleShape vertical;
    vertical.setSize({1.0f, 24.0f});
    vertical.setPosition(position + sf::Vector2f{-0.5f, -12.0f});
    vertical.setFillColor(sf::Color(230, 246, 255, 220));
    window.draw(vertical);
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

    for (int i = 0; i < 10; ++i)
    {
        spawnEnemy();
    }
}