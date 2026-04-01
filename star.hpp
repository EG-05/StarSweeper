#pragma once
#ifndef STARTILE_H
#define STARTILE_H

#include <vector>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

class StarTile {
private:

    int x, y;
    bool is_active;
    bool connected;
    int adjacent_dead;
    //AM
    int adjacent_dead_count;
    float radius;


public:
    //vector storing StarTile objects 
    std::vector<StarTile*> adjacent_stars;
    StarTile* nextStar = nullptr; //linked list connection to next star 

    //AM -- public bec accessing in constellation.cpp 
    bool is_dead;
    bool is_unlocked;
    bool is_unstable;

    StarTile(int a, int b);
    void unlock();
    void markDead();
    void markUnstable();
    void setActive(bool state);
    void addAdjacent(StarTile* neighbor);
    int countAdjacentDead();
    bool canConnectWith(StarTile* other);
    bool isAdjacentTo(StarTile* other);
    int getX() const;
    int getY() const;
    float getRadius() const { return radius; }
    void setRadius(float r) { radius = r; }

    

};

#endif // STARTILE_H