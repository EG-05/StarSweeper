//gameLevel.hpp 
#include "GameLevel.h"
#include "AsteroidTypes.h"
#include <iostream>

GameLevel::GameLevel(int startLevel) : level(startLevel) {
    // Initialize based on starting level
    difficultyMultiplier = 1.0f + ((level - 1) * 0.2f);

    asteroidSpawnInterval = std::max(0.5f, 1.5f - ((level - 1) * 0.2f));
    asteroidSpawnTimer = 0.0f;
}

GameLevel::~GameLevel() {
    // Clean up any allocated resources
    for (auto img : asteroidImages) {
        if (img) {
            al_destroy_bitmap(img);
        }
    }
}

void GameLevel::loadAsteroidImages(std::vector<const char*> imagePaths) {
    // Load multiple asteroid images for variety
    for (const char* path : imagePaths) {
        ALLEGRO_BITMAP* img = al_load_bitmap(path);
        if (img) {
            asteroidImages.push_back(img);
        }
        else {
            std::cerr << "Failed to load asteroid image: " << path << std::endl;
        }
    }

    // If no images were loaded successfully, use a default/fallback
    if (asteroidImages.empty()) {
        std::cerr << "Warning: No asteroid images loaded. Using default." << std::endl;
        ALLEGRO_BITMAP* default_img = al_load_bitmap("asteroid (1).png");
        if (default_img) {
            asteroidImages.push_back(default_img);
        }
    }
}

void GameLevel::update(std::vector<std::unique_ptr<BaseAsteroid>>& asteroids, int screenWidth, int screenHeight) {
    // Update spawn timer
    asteroidSpawnTimer += 1.0f / 60.0f; // Assuming 60 FPS

    // Spawn new asteroids when timer expires

    if (asteroidSpawnTimer >= asteroidSpawnInterval) {
        // Base number increases with level
        int asteroidsToSpawn = 2 + (level / 2);  // Level 1:2, Level 2:3, Level 4:4

        // Cap at 5 asteroids per spawn
        asteroidsToSpawn = std::min(asteroidsToSpawn, 5);

        for (int i = 0; i < asteroidsToSpawn; i++) {
            spawnAsteroid(asteroids, screenWidth, screenHeight);
        }

        asteroidSpawnTimer = 0.0f;
    }
}

void GameLevel::spawnAsteroid(std::vector<std::unique_ptr<BaseAsteroid>>& asteroids, int screenWidth, int screenHeight) {
    // Spawn just off-screen to the right
    int spawnX = screenWidth + (rand() % 100);
    int spawnY = rand() % (screenHeight - 100); // Ensure it's within visible area

    // Create a random asteroid type
    auto newAsteroid = createRandomAsteroid(spawnX, spawnY);
    if (newAsteroid) {
        asteroids.push_back(std::move(newAsteroid));
    }
}

void GameLevel::increaseLevel() {
    level++;
    // Adjust difficulty parameters
    difficultyMultiplier = 1.0f + ((level - 1) * 0.2f);
    asteroidSpawnInterval = std::max(0.5f, 1.5f - ((level - 1) * 0.2f));
    std::cout << "Level increased to " << level << ". New spawn interval: " << asteroidSpawnInterval << std::endl;
}

float GameLevel::getScrollSpeed() {
    // Base speed is 4.0f, increase by 0.5 per level
    //return 4.0f + ((level - 1) * 0.5f);
    return 8.0f + ((level-1) * 0.8f);
}

float GameLevel::getDifficultyMultiplier() const {
    return difficultyMultiplier;
}

std::unique_ptr<BaseAsteroid> GameLevel::createRandomAsteroid(int x, int y) {
    if (asteroidImages.empty()) {
        return nullptr;
    }

    // Pick a random asteroid image
    ALLEGRO_BITMAP* img = asteroidImages[rand() % asteroidImages.size()];

    // Choose a random asteroid type based on level
    // Higher levels have more variety and higher chance of difficult asteroids
    int asteroidType = rand() % 100;

    if (level >= 4 && asteroidType < 25) {
        // 25% chance of Heavy Asteroid in level 4+
        return std::make_unique<HeavyAsteroid>(x, y, img, difficultyMultiplier);
    }
    else if (level >= 3 && asteroidType < 55) {
        // 30% chance of Spinning Asteroid in level 3+
        return std::make_unique<SpinningAsteroid>(x, y, img, difficultyMultiplier);
    }
    else if (level >= 2 && asteroidType < 80) {
        // 25% chance of Zigzag Asteroid in level 2+
        return std::make_unique<ZigzagAsteroid>(x, y, img, difficultyMultiplier);
    }
    else {
        // Standard asteroid always available
        return std::make_unique<StandardAsteroid>(x, y, img, difficultyMultiplier);
    }
}