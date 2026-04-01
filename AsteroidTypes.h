#pragma once
//typesOfasteroid .h
#pragma once
#include "BaseAsteroid.h"

// Regular asteroid - moves in a straight line
class StandardAsteroid : public BaseAsteroid {
public:
    StandardAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier = 1.0f);
    virtual void update() override;
};

// Zig-zag asteroid - changes vertical direction periodically
class ZigzagAsteroid : public BaseAsteroid {
private:
    float directionChangeTimer;
    float directionChangeInterval;
public:
    ZigzagAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier = 1.0f);
    virtual void update() override;
};

// Spinning asteroid - has faster rotation and might be more dangerous
class SpinningAsteroid : public BaseAsteroid {
public:
    SpinningAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier = 1.0f);
    virtual void update() override;
    virtual void applyPhysics() override;
};

// Heavy asteroid - affected by gravity more, falls downward
class HeavyAsteroid : public BaseAsteroid {
public:
    HeavyAsteroid(int startX, int startY, ALLEGRO_BITMAP* asteroidImg, float speedMultiplier = 1.0f);
    virtual void update() override;
};