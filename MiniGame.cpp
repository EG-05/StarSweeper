#include "MiniGame.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <ctime>
#include <string>
#include <algorithm>
#include "AsteroidTypes.h" // Include asteroid types

MiniGame::MiniGame() {
    initAllegro();
    loadAssets();
}

MiniGame::~MiniGame() {
    cleanup();
}

void MiniGame::initAllegro() {
    al_init();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(3);
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
}

void MiniGame::loadAssets() {
    // Create display with error checking
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display!" << std::endl;
        exit(1);
    }

    // Create timer with error checking
    timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        std::cerr << "Failed to create timer!" << std::endl;
        exit(1);
    }

    // Create event queue with error checking
    eventQueue = al_create_event_queue();
    if (!eventQueue) {
        std::cerr << "Failed to create event queue!" << std::endl;
        exit(1);
    }

    // Register event sources with error checking
    if (!al_get_display_event_source(display)) {
        std::cerr << "Failed to get display event source!" << std::endl;
        exit(1);
    }
    al_register_event_source(eventQueue, al_get_display_event_source(display));

    if (!al_get_keyboard_event_source()) {
        std::cerr << "Failed to get keyboard event source!" << std::endl;
        exit(1);
    }
    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    if (!al_get_timer_event_source(timer)) {
        std::cerr << "Failed to get timer event source!" << std::endl;
        exit(1);
    }
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    // Load background image with error checking
    backgroundImg = al_load_bitmap("minibackground.jpg");
    if (!backgroundImg) {
        std::cerr << "Failed to load background image: minibackground.jpg" << std::endl;
        exit(1);
    }

    // Load finish line image with error checking
    finishLineImg = al_load_bitmap("finish_line.png");
    if (!finishLineImg) {
        std::cerr << "Failed to load finish line image: finish_line.png" << std::endl;
        exit(1);
    }

    // Setup asteroid image paths
    asteroidImagePaths = {
        "planet1.png",
        "planet2.png",
        "planet3.png",
        "planet4.png",
        "earth1 (1).png"
    };

    // Load player animation frames with error checking
    bool playerImagesLoaded = false;
    for (int i = 0; i < 13; ++i) {
        std::string filename = std::to_string(i) + ".png";
        ALLEGRO_BITMAP* frame = al_load_bitmap(filename.c_str());
        if (frame) {
            playerImages.push_back(frame);
            playerImagesLoaded = true;
        }
        else {
            std::cerr << "Warning: Failed to load player image: " << filename << std::endl;
        }
    }

    if (!playerImagesLoaded) {
        std::cerr << "Error: No player images could be loaded!" << std::endl;
        exit(1);
    }

    // Load font with error checking
    font = al_load_ttf_font("Tagesschrift.ttf", 38, 0);
    if (!font) {
        std::cerr << "Failed to load font: Tagesschrift.ttf" << std::endl;
        exit(1);
    }

    // Load sound files with error checking
    explosionSound = al_load_sample("explosion.wav");
    if (!explosionSound) {
        std::cerr << "Warning: Failed to load sound: explosion.wav" << std::endl;
    }

    miniGameMusic = al_load_sample("MiniGameMusic.wav");
    if (!miniGameMusic) {
        std::cerr << "Warning: Failed to load sound: MiniGameMusic.wav" << std::endl;
    }
    else {
        // Only set up music if loading succeeded
        bgMusicInstance = al_create_sample_instance(miniGameMusic);
        if (!bgMusicInstance) {
            std::cerr << "Warning: Failed to create sample instance" << std::endl;
        }
        else {
            al_attach_sample_instance_to_mixer(bgMusicInstance, al_get_default_mixer());
            al_set_sample_instance_playmode(bgMusicInstance, ALLEGRO_PLAYMODE_LOOP);
            al_play_sample_instance(bgMusicInstance);
        }
    }

    wompWomp = al_load_sample("WompWomp.wav");
    if (!wompWomp) {
        std::cerr << "Warning: Failed to load sound: WompWomp.wav" << std::endl;
    }

    // Create player with animation frames
    if (!playerImages.empty()) {
        player = new Player(0, SCREEN_HEIGHT / 2, playerImages);
    }
    else {
        std::cerr << "Error: Cannot create player without images" << std::endl;
        exit(1);
    }

    // Initialize game level
    gameLevel = new GameLevel(currentLevel);
    gameLevel->loadAsteroidImages(asteroidImagePaths);

    srand(time(nullptr));
}

void MiniGame::run() {
    // Start the game timer
    al_start_timer(timer);

    // Main game loop
    while (running) {
        handleEvents();

        if (redraw && al_is_event_queue_empty(eventQueue)) {
            renderGame();
        }
    }

    // Show end screen with result
    showEndScreen();

    // We need to properly close the window now
    //cleanup();
}

bool MiniGame::survived() {
    return reachedFinish && player->isAlive;
}

void MiniGame::handleEvents() {
    ALLEGRO_EVENT ev;
    al_wait_for_event(eventQueue, &ev);

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        running = false;
    }
    else if (ev.type == ALLEGRO_EVENT_TIMER) {
        if (!gameOver && !levelComplete) {
            frameCount++;

            al_get_keyboard_state(&keyState);
            bool up = al_key_down(&keyState, ALLEGRO_KEY_UP);
            bool down = al_key_down(&keyState, ALLEGRO_KEY_DOWN);

            // Scroll background
            float scrollSpeed = gameLevel->getScrollSpeed();

            // Apply some acceleration to the camera for smoother movement
            static float currentScrollSpeed = 1.5f + scrollSpeed;
            currentScrollSpeed = currentScrollSpeed * 0.95f + scrollSpeed * 0.15f;

            cameraX += currentScrollSpeed;

            // Update player with speed multiplier
            player->setSpeedMultiplier(gameLevel->getDifficultyMultiplier());
            player->update(up, down);

            // End level after time runs out
            if (frameCount >= targetFrames) {
                levelComplete = true;
                reachedFinish = true;
                running = false; // Stop the game loop when level is complete
            }

            // Spawn and update asteroids
            gameLevel->update(asteroids, SCREEN_WIDTH, SCREEN_HEIGHT);
            for (auto& asteroid : asteroids) {
                asteroid->applyPhysics();
                asteroid->update();

                if (player->checkCollisionWithAsteroid(*asteroid)) {
                    player->isAlive = false;
                    gameOver = true;
                    levelComplete = true;
                    running = false; // Stop the game loop on collision

                    // Stop music and play failure sound
                    al_stop_sample_instance(bgMusicInstance);
                    al_play_sample(wompWomp, 5.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
                    break;
                }
            }

            // Remove off-screen asteroids
            asteroids.erase(
                std::remove_if(asteroids.begin(), asteroids.end(),
                    [&](const std::unique_ptr<BaseAsteroid>& ast) {
                        return ast->isOutOfBounds(SCREEN_WIDTH, SCREEN_HEIGHT);
                    }),
                asteroids.end()
            );
        }

        redraw = true;
    }
    //else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
    //    // If game is over and user presses space, exit the mini-game
    //    if ((gameOver || levelComplete) && ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
    //        running = false;
    //    }
    //}
}

void MiniGame::renderGame() {
    redraw = false;
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // Draw scrolling background
    float offsetX = -fmod(cameraX, (float)al_get_bitmap_width(backgroundImg));
    al_draw_bitmap(backgroundImg, offsetX, 0, 0);
    al_draw_bitmap(backgroundImg, offsetX + al_get_bitmap_width(backgroundImg), 0, 0);

    // Draw player if alive
    if (player->isAlive) {
        player->draw();
    }

    // Draw all asteroids
    for (auto& ast : asteroids) {
        ast->draw();
    }

    // Draw UI elements
    int secondsLeft = std::max(0, (targetFrames - frameCount) / 60);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Level: %d", currentLevel);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 50, 0, "Time left: %d", secondsLeft);

    // Show game over or level complete messages
    if (gameOver) {
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
        //al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTER, "Press SPACE to continue");
    }
    else if (levelComplete) {
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "LEVEL COMPLETE!");
        //al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTER, "Press SPACE to continue");
    }

    al_flip_display();
}

void MiniGame::showEndScreen() {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (reachedFinish && player->isAlive) {
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "YOU WIN!");
        //al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTER, "Press SPACE to continue");
        al_rest(1.0);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, ALLEGRO_ALIGN_CENTER, "Press SPACE to continue");

    }
    else {
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "YOU DIED");
        //al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTER, "Press SPACE to continue");
        al_rest(1.0);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, ALLEGRO_ALIGN_CENTER, "Press SPACE to restart");
    }

    al_flip_display();

    al_rest(1.0);
}

void MiniGame::cleanup() {
    // Clear out all the remaining events
    al_flush_event_queue(eventQueue);

    // Stop any playing sounds
    if (bgMusicInstance) {
        al_stop_sample_instance(bgMusicInstance);
        al_destroy_sample_instance(bgMusicInstance);
        bgMusicInstance = nullptr;
    }

    // Delete objects
    if (player) {
        delete player;
        player = nullptr;
    }

    if (gameLevel) {
        delete gameLevel;
        gameLevel = nullptr;
    }

    // Destroy allegro resources
    if (display) {
        al_destroy_display(display);
        display = nullptr;
    }

    if (timer) {
        al_destroy_timer(timer);
        timer = nullptr;
    }

    if (eventQueue) {
        al_destroy_event_queue(eventQueue);
        eventQueue = nullptr;
    }

    if (backgroundImg) {
        al_destroy_bitmap(backgroundImg);
        backgroundImg = nullptr;
    }

    if (finishLineImg) {
        al_destroy_bitmap(finishLineImg);
        finishLineImg = nullptr;
    }

    if (font) {
        al_destroy_font(font);
        font = nullptr;
    }

    if (explosionSound) {
        al_destroy_sample(explosionSound);
        explosionSound = nullptr;
    }

    if (miniGameMusic) {
        al_destroy_sample(miniGameMusic);
        miniGameMusic = nullptr;
    }

    if (wompWomp) {
        al_destroy_sample(wompWomp);
        wompWomp = nullptr;
    }

    // Clean up player images
    for (auto& img : playerImages) {
        if (img) {
            al_destroy_bitmap(img);
        }
    }
    playerImages.clear();

    // Clear asteroids
    asteroids.clear();
}