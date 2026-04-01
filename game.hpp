#ifndef GAME_HPP // Use a unique name
#define GAME_HPP
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "constellation.hpp"
#include "MiniGame.h"


#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

class Game {
private:
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* eventQueue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_BITMAP* background;
    bool running;
    Constellation* constellation;
    ALLEGRO_BITMAP* starImage;

    ALLEGRO_SAMPLE_INSTANCE* bgMusicInstance = nullptr;
    ALLEGRO_SAMPLE* MainMusic = nullptr;
    //EG END

    int mouseX, mouseY;

public:
    Game();
    ~Game();
    bool init();
    void run();
    //void update();
    void render();
    void cleanup();
    void gameOver();
    void waitForSpaceToRestart();
    //void reinitializeAfterMiniGame();
    //void resumeFromMiniGame(bool survived, StarTile* star);
    void miniGameLost();

    //NEW 
    void restoreAfterMiniGame();
    void showMessage(const char* text, ALLEGRO_COLOR color, float duration);

};

#endif