//base asteroid cpp 
#include "BaseAsteroid.h"
using namespace std;

BaseAsteroid::BaseAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg)
    : x(startX), y(startY), image(asteroidImg), isActive(true)
{
    width = al_get_bitmap_width(asteroidImg);
    height = al_get_bitmap_height(asteroidImg);

    velocityX = -((rand() % 5) + 5);  // Faster than camera scroll speed (3.0f)
    velocityY = ((rand() % 3) - 1);   // -1 to 1

    rotation = 0;
    //angularVelocity = 0.05f + static_cast<float>(rand() % 10) / 100.0f;
    angularVelocity = 0;
    gravity = 0;

    // Initialize collision radius (circular collision for asteroids)
    // Use 40% of the smallest dimension for a more accurate collision
    collisionRadius = min(width, height) * 0.4f;
}

//void BaseAsteroid::update() {
//    x += velocityX;
//    y += velocityY;
//
//    // Do not respawn here - let the main game handle asteroid lifecycle
//}

void BaseAsteroid::applyPhysics() {
    velocityY += gravity;
    rotation += angularVelocity;
}

void BaseAsteroid::draw() {
    if (isActive) {
        al_draw_rotated_bitmap(image,
            width / 2, height / 2,
            x + width / 2, y + height / 2,
            rotation, 0);

        // al_draw_circle(x + width/2, y + height/2, collisionRadius, al_map_rgb(255, 0, 0), 2.0);
    }
}

bool BaseAsteroid::isOutOfBounds(int screenWidth, int screenHeight) const {
    // Consider asteroid out of bounds when it's completely off the screen
    return (x + width < -100) || (x > screenWidth + 100) ||
        (y + height < -100) || (y > screenHeight + 100);
}

//COLLISION CHECKSSS
// Get center X coordinate
float BaseAsteroid::getCenterX() const {
    return x + width / 2;
}

// Get center Y coordinate
float BaseAsteroid::getCenterY() const {
    return y + height / 2;
}

// Get collision radius
float BaseAsteroid::getCollisionRadius() const {
    return collisionRadius;
}


