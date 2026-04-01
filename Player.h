// player h 
#pragma once
#include <allegro5/allegro.h>
#include <vector>
#include "BaseAsteroid.h"

// Forward declaration to avoid circular dependency
class BaseAsteroid;

class Player {
public:
    // Support both single image and animation frames
    Player(int startX, int startY, ALLEGRO_BITMAP* img);
    Player(int startX, int startY, const std::vector<ALLEGRO_BITMAP*>& images);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    float getVelocityPX() const;
    float getVelocityPY() const;

    void setX(int newX);
    void setY(int newY);
    void setVelocityPX(float vx);
    void setVelocityPY(float vy);
    void setSpeedMultiplier(float multiplier);
    void setLevel(int newLevel) { level = newLevel; }

    // Game logic
    void update(bool upPressed, bool downPressed);
    void resetPosition(int startX, int startY);
    void draw();
    bool checkCollisionWithAsteroid(BaseAsteroid& asteroid);

    bool isAlive;
    void initializeCollisionBoundary();

private:
    int x, y;
    int width, height;

    // Collision boundary (smaller than the full bitmap)
    float collisionBoundaryOffsetX;
    float collisionBoundaryOffsetY;
    float collisionBoundaryWidth;
    float collisionBoundaryHeight;


    // Support both single image and animation
    ALLEGRO_BITMAP* image = nullptr;
    std::vector<ALLEGRO_BITMAP*> images;

    // Animation properties
    int currentFrame = 0;
    float animationSpeed = 0.1f;
    float animationTimer = 0.0f;

    // Movement properties
    float baseSpeed = 5.0f;
    float speedMultiplier = 1.0f;
    int level = 1;
    float velocityPX = 2.57f;  // Constant forward movement
    float velocityPY = 0.0f;   // Vertical movement via keys
};
