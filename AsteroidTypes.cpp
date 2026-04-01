//asteroid types.cpp 
#include "AsteroidTypes.h"
#include <cmath>

// StandardAsteroid Implementation
StandardAsteroid::StandardAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier)
    : BaseAsteroid(startX, startY, asteroidImg)
{
    // Base speed adjusted by level multiplier
    //velocityX = -((rand() % 5) + 5) * speedMultiplier;
    //velocityY = ((rand() % 3) - 1) * speedMultiplier;

    //// Base speed adjusted by level multiplier
    //velocityX = -((rand() % 5) + 5) * speedMultiplier; // Include speedMultiplier
    //velocityY = ((rand() % 3) - 1) * speedMultiplier; // Include speedMultiplier


    //// Moderate rotation
    //angularVelocity = (0.02f + static_cast<float>(rand() % 6) / 100.0f) * speedMultiplier;
    
     // Increased base speed with higher multiplier for faster gameplay
    velocityX = -((rand() % 7) + 8) * speedMultiplier; // Faster horizontal speed
    velocityY = ((rand() % 4) - 2) * speedMultiplier; // More vertical movement potential

    // Slightly faster rotation for more dynamic appearance
    angularVelocity = (0.04f + static_cast<float>(rand() % 8) / 100.0f) * speedMultiplier;
}

void StandardAsteroid::update() {
    x += velocityX;
    y += velocityY;
}

// ZigzagAsteroid Implementation
ZigzagAsteroid::ZigzagAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier)
    : BaseAsteroid(startX, startY, asteroidImg)
{
    /*velocityX = -((rand() % 4) + 4) * speedMultiplier;
    velocityY = 2.0f * speedMultiplier;*/

    velocityX = -((rand() % 4) + 4) * speedMultiplier; // NEW: Include speedMultiplier
    velocityY = 2.0f * speedMultiplier; // NEW: Include speedMultiplier

    // Moderate rotation
    angularVelocity = (0.03f + static_cast<float>(rand() % 5) / 100.0f) * speedMultiplier;

    // Set up direction change timer
    directionChangeInterval = 0.5f + static_cast<float>(rand() % 10) / 20.0f; // 0.5 to 1.0 seconds
    directionChangeTimer = 0.0f;
}

void ZigzagAsteroid::update() {
    x += velocityX;
    y += velocityY;

    // Update direction change timer
    directionChangeTimer += 1.0f / 60.0f; // Assuming 60 FPS

    // Change direction when the timer expires
    if (directionChangeTimer >= directionChangeInterval) {
        velocityY = -velocityY;
        directionChangeTimer = 0.0f;
    }
}

// SpinningAsteroid Implementation
SpinningAsteroid::SpinningAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier)
    : BaseAsteroid(startX, startY, asteroidImg)
{
    /*velocityX = -((rand() % 6) + 6) * speedMultiplier;
    velocityY = ((rand() % 5) - 2) * speedMultiplier;*/

    velocityX = -((rand() % 6) + 6) * speedMultiplier; // NEW: Include speedMultiplier
    velocityY = ((rand() % 5) - 2) * speedMultiplier; // NEW: Include speedMul

    // Fast rotation is the defining characteristic
    angularVelocity = (0.1f + static_cast<float>(rand() % 15) / 100.0f) * speedMultiplier;

    //velocityX = -((rand() % 5) + 5) * speedMultiplier; // Include speedMultiplier
    //velocityY = ((rand() % 3) - 1) * speedMultiplier; // Include speedMultiplier
    //angularVelocity = (0.02f + static_cast<float>(rand() % 6) / 100.0f) * speedMultiplier;
}

void SpinningAsteroid::update() {
    x += velocityX;
    y += velocityY;
}

void SpinningAsteroid::applyPhysics() {
    // Override with faster rotation
    rotation += angularVelocity * 1.5f;
}

// HeavyAsteroid Implementation
HeavyAsteroid::HeavyAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier)
    : BaseAsteroid(startX, startY, asteroidImg)
{
    velocityX = -((rand() % 4) + 3) * speedMultiplier; // Slightly slower horizontal speed
    velocityY = ((rand() % 2)) * speedMultiplier;      // Initial downward or neutral

    // Slower rotation but affected by gravity
    angularVelocity = (0.01f + static_cast<float>(rand() % 4) / 100.0f) * speedMultiplier;
    gravity = 0.05f * speedMultiplier;
}

void HeavyAsteroid::update() {
    x += velocityX;
    y += velocityY;
}