#pragma once
//hpp
#ifndef MINIGAME_H
#define MINIGAME_H

#include <vector>
#include <memory> // For std::unique_ptr
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "Player.h"
#include "BaseAsteroid.h" // Changed from Asteroid to BaseAsteroid
#include "GameLevel.h"    // Added GameLevel

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

class MiniGame {
public:
    MiniGame();
    ~MiniGame();

    void run();
    bool survived();

private:
    void initAllegro();
    void loadAssets();
    void handleEvents();
    void renderGame();
    void cleanup();
    void showEndScreen();
    bool getNextLevel(); 


    ALLEGRO_DISPLAY* display = nullptr;
    ALLEGRO_EVENT_QUEUE* eventQueue = nullptr;
    ALLEGRO_TIMER* timer = nullptr;

    ALLEGRO_FONT* font = nullptr;
    ALLEGRO_BITMAP* backgroundImg = nullptr;
    ALLEGRO_BITMAP* asteroidImg = nullptr;
    ALLEGRO_BITMAP* finishLineImg = nullptr; // Added finish line
    std::vector<ALLEGRO_BITMAP*> playerImages;
    std::vector<const char*> asteroidImagePaths; // Store paths for GameLevel

    ALLEGRO_SAMPLE* explosionSound = nullptr;
    ALLEGRO_SAMPLE* miniGameMusic = nullptr;
    ALLEGRO_SAMPLE* wompWomp = nullptr;
    ALLEGRO_SAMPLE_INSTANCE* bgMusicInstance = nullptr;

    ALLEGRO_KEYBOARD_STATE keyState;

    Player* player = nullptr;

    // Using BaseAsteroid with polymorphism and GameLevel
    std::vector<std::unique_ptr<BaseAsteroid>> asteroids;
    GameLevel* gameLevel = nullptr;

    bool running = true;
    bool redraw = true;
    bool reachedFinish = false;
    bool gameOver = false;     // Added game over state
    bool levelComplete = false; // Added level complete state

    int frameCount = 0;
    int currentLevel = 1;      // Track current level
    float cameraX = 0.0f;

    const int targetFrames = 10 * 60; // 10 seconds * 60 FPS
    const float finishLineX = 3000.0f;
};

#endif

