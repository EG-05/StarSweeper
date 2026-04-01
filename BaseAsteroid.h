//base asteroid 
#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <algorithm>

class BaseAsteroid {
protected:
    float x, y;
    float velocityX, velocityY;
    int width, height;
    bool isActive;
    ALLEGRO_BITMAP* image;
    float gravity;
    float angularVelocity;
    float rotation;
    float collisionRadius;  // Added for better collision detection

public:
    //Asteroid(int screenWidth, int screenHeight, ALLEGRO_BITMAP* asteroidImg);
    BaseAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg);  // Change constructor signature

    // Virtual destructor for derived classes
    virtual ~BaseAsteroid() {}

    virtual void update() = 0;
    virtual void draw();

    virtual void applyPhysics();
    bool isOutOfBounds(int screenWidth, int screenHeight) const;

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool getIsActive() const { return isActive; }
    float getRotation() const { return rotation; }
    float getCenterX() const;  // Added to get center of asteroid
    float getCenterY() const;  // Added to get center of asteroid
    float getCollisionRadius() const;  // Added for circular collision

    // Setters
    void setIsActive(bool active) { isActive = active; }
    void setGravity(float g) { gravity = g; }
    void setVelocityX(float vx) { velocityX = vx; }
};