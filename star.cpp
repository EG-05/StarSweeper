#include <iostream>
using namespace std;

#include <vector>
#include "star.hpp" 

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

/*
Get width and get height for the image

*/


StarTile::StarTile(int a, int b) {
    x = a;
    y = b;
    is_dead = false;
    is_unlocked = false;
    is_active = false;
    connected = false;
    adjacent_dead = 0;
    adjacent_dead_count=0;
    radius = /*2.0f + static_cast<float>(rand() % 1)+2;*/ ((rand() % 26) / 10.0f) + 2.0f;
}

void StarTile::unlock() {
    if (is_dead) return;

    if (is_unstable) {
        // Don't unlock here — signal that a mini-game should be triggered instead
        std::cout << "Unstable star clicked! Launching mini-game...\n";
        // This should be handled by main game logic elsewhere
        return;
    }

    is_unlocked = true;
    is_active = true;
}

void StarTile::markDead() {
    is_dead = true;
    is_active = false;
    is_unlocked = true;
}

void StarTile::markUnstable() {
    is_dead = false;
    is_active = false;
    is_unlocked = true;
    is_unstable = true;
}

void StarTile::setActive(bool state) {
    is_active = state;
    if (state == true) {
        is_unlocked = true;
    }
    else {
        is_unlocked = false;
    }
}

//neighbour includes the coordinates too
void StarTile::addAdjacent(StarTile* neighbor) {
    adjacent_stars.push_back(neighbor);
}

// To show how many are adjacent dead and if one is active 
int StarTile::countAdjacentDead() {
    int count = 0;
    for (StarTile* neighbor : adjacent_stars) {
        if (neighbor->is_dead) {
            count++;
        }
    }
    adjacent_dead_count = count;
    return adjacent_dead_count;
}

bool StarTile::canConnectWith(StarTile* other) {
    if (other == nullptr) {
        return false;
    }

    if (this->is_unlocked && other->is_unlocked && this->isAdjacentTo(other)) {
        return true;
    }
    else {
        return false;
    }
}

bool StarTile::isAdjacentTo(StarTile* other) {
    for (StarTile* neighbor : adjacent_stars) {
        if (neighbor == other) return true;
    }
    return false;
}

int StarTile::getX() const {
    return x;
}

int StarTile::getY() const {
    return y;
}