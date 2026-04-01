//gameLvel.h 
#pragma once
#include <vector>
#include <memory>
#include <allegro5/allegro.h>
#include "BaseAsteroid.h"

class GameLevel {
private:
    int level;
    float difficultyMultiplier;
    float asteroidSpawnInterval;
    float asteroidSpawnTimer;

    // Multiple asteroid images support
    std::vector<ALLEGRO_BITMAP*> asteroidImages;

public:
    GameLevel(int startLevel = 1);
    ~GameLevel();

    // Load resources
    void loadAsteroidImages(std::vector<const char*> imagePaths);

    // Game logic
    void update(std::vector<std::unique_ptr<BaseAsteroid>>& asteroids, int screenWidth, int screenHeight);
    void spawnAsteroid(std::vector<std::unique_ptr<BaseAsteroid>>& asteroids, int screenWidth, int screenHeight);

    // Level management
    void increaseLevel();
    int getLevel() const { return level; }
    float getDifficultyMultiplier() const;
    float getScrollSpeed() ; // Speed increases with level

    // Create different asteroid types
    std::unique_ptr<BaseAsteroid> createRandomAsteroid(int x, int y);
};