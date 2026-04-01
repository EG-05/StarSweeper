// player cpp
#include "Player.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

// Constructor for single image (your original implementation)
Player::Player(int startX, int startY, ALLEGRO_BITMAP* img) {
    x = startX;
    y = startY;
    image = img;
    isAlive = true;
    width = al_get_bitmap_width(image);
    height = al_get_bitmap_height(image);

    // Initialize collision boundaries (smaller than full bitmap)
    initializeCollisionBoundary();

    baseSpeed = 5.0f;
    speedMultiplier = 1.0f;
    level = 1;
}

// Constructor for animation frames (your friend's implementation)
Player::Player(int startX, int startY, const std::vector<ALLEGRO_BITMAP*>& images)
    : x(startX), y(startY), images(images), currentFrame(0), animationSpeed(0.1f), animationTimer(0.0f), width(0), height(0) {

    if (!images.empty()) {
        width = al_get_bitmap_width(images[0]);
        height = al_get_bitmap_height(images[0]);
    }
    isAlive = true;

    // Initialize collision boundaries (smaller than full bitmap)
    initializeCollisionBoundary();

    baseSpeed = 5.0f;
    speedMultiplier = 1.0f;
    level = 1;
}

//COLLISION BOUNDARY 
void Player::initializeCollisionBoundary() {
    // Create a collision boundary smaller than the actual image
    // These percentages can be adjusted based on your specific sprites
    collisionBoundaryOffsetX = width * 0.2f;  // 20% margin from left/right
    collisionBoundaryOffsetY = height * 0.2f; // 20% margin from top/bottom
    collisionBoundaryWidth = width * 0.6f;    // 60% of original width
    collisionBoundaryHeight = height * 0.6f;  // 60% of original height
}

// Getters
int Player::getX() const { return x; }
int Player::getY() const { return y; }
int Player::getWidth() const { return width; }
int Player::getHeight() const { return height; }
float Player::getVelocityPX() const { return velocityPX; }
float Player::getVelocityPY() const { return velocityPY; }

// Setters
void Player::setX(int newX) { x = newX; }
void Player::setY(int newY) { y = newY; }
void Player::setVelocityPX(float vx) { velocityPX = vx; }
void Player::setVelocityPY(float vy) { velocityPY = vy; }
void Player::setSpeedMultiplier(float multiplier) {
    speedMultiplier = multiplier;
}

void Player::update(bool upPressed, bool downPressed) {
    // Constant horizontal motion (moving forward)
    //MOVING FORWARDS
    velocityPX = 2.0f * speedMultiplier; // Allow player to move forward faster
    x += velocityPX;

    // Calculate speed based on level and multiplier
    //float adjustedSpeed = (baseSpeed + (level - 1) * 0.5f) * speedMultiplier;
    float adjustedSpeed = (baseSpeed + (level - 1) * 0.8f) * speedMultiplier;

    // More responsive vertical movement
    const float maxVelocity = 8.0f * speedMultiplier;
    const float acceleration = 0.98f * speedMultiplier;
    const float deceleration = 0.6f * speedMultiplier;

    // Merged approach: Support both direct movement and velocity-based movement
    if (upPressed) {
        y -= adjustedSpeed;  // Direct movement
        //velocityPY = -5.0f;  // Velocity-based
        velocityPY = std::max(velocityPY - acceleration, -maxVelocity);

    }
    else if (downPressed) {
        y += adjustedSpeed;  // Direct movement (your implementation)
        //velocityPY = 5.0f;   // Velocity
        velocityPY = std::min(velocityPY + acceleration, maxVelocity);

    }
    else {
       /* velocityPY = 0.0f;*/
        // Decelerate when no keys are pressed (smoother stopping)
        if (velocityPY > 0) {
            velocityPY = std::max(0.0f, velocityPY - deceleration);
        }
        else if (velocityPY < 0) {
            velocityPY = std::min(0.0f, velocityPY + deceleration);
        }
    }

    // Apply vertical velocity (this will effectively double the movement in the
    // current implementation, but retaining both for compatibility)
    y += velocityPY; //??

    // Screen bounds
    if (y < 0) { 
        y = 0;
        velocityPY = std::abs(velocityPY) * 0.5f; // Small bounce
    }
    if (y + height > 800){ 
        y = 800 - height;  // Assuming screen height is 800
        velocityPY = -std::abs(velocityPY) * 0.5f; // Small bounce

    }
    if (x + width > 1800) {
        x = 1800 - width;  // Keep the player fully on screen
        velocityPX = 0;
    }

    // Update animation frame if using multiple images
    if (!images.empty()) {
        animationTimer += animationSpeed;
        if (animationTimer >= 1.0f) { // Change frame every second
            currentFrame = (currentFrame + 1) % images.size(); // Loop through frames
            animationTimer = 0.0f;
        }
    }
}

void Player::resetPosition(int startX, int startY) {
    x = startX;
    y = startY;
    velocityPX = 0.0f;
    velocityPY = 0.0f;
    isAlive = true;
    speedMultiplier = 1.0f;
    level = 1;
}


//bool Player::checkCollisionWithAsteroid(BaseAsteroid& asteroid) {
//    bool xOverlap = !(x + width <= asteroid.getX() || x >= asteroid.getX() + asteroid.getWidth());
//    bool yOverlap = !(y + height <= asteroid.getY() || y >= asteroid.getY() + asteroid.getHeight());
//    return xOverlap && yOverlap;
//}

bool Player::checkCollisionWithAsteroid(BaseAsteroid& asteroid) {
    // Get the collision boundaries for player and asteroid
    float playerCollisionX = x + collisionBoundaryOffsetX;
    float playerCollisionY = y + collisionBoundaryOffsetY;

    // For asteroid, also use reduced collision area (60% of original size)
    float asteroidX = asteroid.getX();
    float asteroidY = asteroid.getY();
    float asteroidWidth = asteroid.getWidth();
    float asteroidHeight = asteroid.getHeight();

    float asteroidCollisionX = asteroidX + (asteroidWidth * 0.2f);
    float asteroidCollisionY = asteroidY + (asteroidHeight * 0.2f);
    float asteroidCollisionWidth = asteroidWidth * 0.6f;
    float asteroidCollisionHeight = asteroidHeight * 0.6f;

    // Check if the collision boundaries overlap
    bool xOverlap = !(playerCollisionX + collisionBoundaryWidth <= asteroidCollisionX ||
        playerCollisionX >= asteroidCollisionX + asteroidCollisionWidth);

    bool yOverlap = !(playerCollisionY + collisionBoundaryHeight <= asteroidCollisionY ||
        playerCollisionY >= asteroidCollisionY + asteroidCollisionHeight);


    return xOverlap && yOverlap;
}

void Player::draw() {
    if (isAlive) {
        if (!images.empty()) {
            // Draw animation frame if we have multiple images
            al_draw_bitmap(images[currentFrame], x, y, 0);
        }
        else if (image) {
            // Draw single image if that's what we have
            al_draw_bitmap(image, x, y, 0);
        }
        // Uncomment for debugging collision boundaries
        // al_draw_rectangle(x + collisionBoundaryOffsetX, y + collisionBoundaryOffsetY, 
        //                   x + collisionBoundaryOffsetX + collisionBoundaryWidth, 
        //                   y + collisionBoundaryOffsetY + collisionBoundaryHeight, 
        //                   al_map_rgb(255, 0, 0), 2.0);
    }
}
