#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "star.hpp"
//#include "game.hpp"

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

class Constellation {
private:
    std::vector<StarTile*> stars;
    StarTile* lastUnlocked = nullptr;
    //ALLEGRO_BITMAP* spriteStar;
    std::vector<StarTile*> revealedStars;

    


public:
    Constellation(/*ALLEGRO_BITMAP* sprite*/);
    ~Constellation();

    std::vector<StarTile*> links;
    const std::vector<StarTile*>& getStars() const { return stars; }

    void generateAllStars();
    void revealStar(StarTile* star);
    void unlockStar(StarTile* clicked);
    int countAdjacentDeadStars(StarTile* center);
    void drawConstellation(int mouseX, int mouseY/*, ALLEGRO_FONT* fontK*/); // Draw stars + lines between them
    void unlockAfterMiniGame(StarTile* unstableStar);
    float distance(StarTile* a, StarTile* b);
};

#endif