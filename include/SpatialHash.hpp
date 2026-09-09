#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>

class SpatialHash
{
public:
    explicit SpatialHash(float cellSize = 80.0f)
        : cellSize(cellSize)
    {
    }

    void clear()
    {
        grid.clear();
    }

    void insert(int id, const sf::FloatRect& bounds)
    {
        int minX = cellX(bounds.position.x);
        int maxX = cellX(bounds.position.x + bounds.size.x);

        int minY = cellY(bounds.position.y);
        int maxY = cellY(bounds.position.y + bounds.size.y);

        for (int x = minX; x <= maxX; ++x)
        {
            for (int y = minY; y <= maxY; ++y)
            {
                grid[hash(x, y)].push_back(id);
            }
        }
    }

    std::vector<int> query(const sf::FloatRect& bounds) const
    {
        std::vector<int> result;
        query(bounds, result);
        return result;
    }

    void query(
        const sf::FloatRect& bounds,
        std::vector<int>& result
    ) const
    {
        result.clear();

        int minX = cellX(bounds.position.x);
        int maxX = cellX(bounds.position.x + bounds.size.x);

        int minY = cellY(bounds.position.y);
        int maxY = cellY(bounds.position.y + bounds.size.y);

        for (int x = minX; x <= maxX; ++x)
        {
            for (int y = minY; y <= maxY; ++y)
            {
                auto it = grid.find(hash(x, y));

                if (it == grid.end())
                    continue;

                for (int id : it->second)
                {
                    result.push_back(id);
                }
            }
        }

        std::sort(result.begin(), result.end());
        result.erase(
            std::unique(result.begin(), result.end()),
            result.end()
        );
    }

    std::size_t cellCount() const
    {
        return grid.size();
    }

private:
    float cellSize;

    std::unordered_map<long long, std::vector<int>> grid;

    int cellX(float x) const
    {
        return static_cast<int>(std::floor(x / cellSize));
    }

    int cellY(float y) const
    {
        return static_cast<int>(std::floor(y / cellSize));
    }

    long long hash(int x, int y) const
    {
        return (static_cast<long long>(x) << 32) ^
               static_cast<unsigned int>(y);
    }
};