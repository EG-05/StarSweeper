#include <iostream>
#include "game.hpp"
#include "constellation.hpp"
#include "MiniGame.h"

#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>


//1920 by 1080

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60


Game::Game()
    : display(nullptr), eventQueue(nullptr), timer(nullptr), running(false), background(nullptr), constellation(nullptr), starImage(nullptr), mouseX(0), mouseY(0) {
}



Game::~Game() {
    cleanup();
}


bool Game::init() {

    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro!\n";
        return false;
    }


    if (!al_install_keyboard()) {
        std::cerr << "Failed to install keyboard!\n";
        return false;
    }


    if (!al_install_mouse()) {
        std::cerr << "Failed to install mouse!\n";
        return false;
    }


    if (!al_init_image_addon()) {
        std::cerr << "Failed to initialize image addon!\n";
        return false;
    }


    al_init_font_addon();
    al_init_primitives_addon();// Must be added to init()
    al_init_ttf_addon();

    al_install_audio();
    al_init_acodec_addon();

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);


    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display!\n";
        return false;
    }


    eventQueue = al_create_event_queue();
    if (!eventQueue) {
        std::cerr << "Failed to create event queue!\n";
        return false;
    }


    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        std::cerr << "Failed to create timer!\n";
        return false;
    }


    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));


    background = al_load_bitmap("background6.jpg");
    if (!background) {
        std::cerr << "Failed to load background image!\n";
        return false;
    }

    if (!al_install_audio()) {
        std::cerr << "Failed to install audio!\n";
        return false;
    }

    if (!al_init_acodec_addon()) {
        std::cerr << "Failed to initialize audio codecs!\n";
        return false;
    }

    if (!al_reserve_samples(1)) {
        std::cerr << "Failed to reserve audio samples!\n";
        return false;
    }

    //EG START 
    MainMusic = al_load_sample("MainMusic.wav");
    if (!MainMusic) {
        std::cerr << "Warning: Failed to load sound: MiniGameMusic.wav" << std::endl;
    }
    else {
        // Only set up music if loading succeeded
        bgMusicInstance = al_create_sample_instance(MainMusic);
        if (!bgMusicInstance) {
            std::cerr << "Warning: Failed to create sample instance" << std::endl;
        }
        else {
            al_attach_sample_instance_to_mixer(bgMusicInstance, al_get_default_mixer());
            al_set_sample_instance_playmode(bgMusicInstance, ALLEGRO_PLAYMODE_LOOP);
            al_play_sample_instance(bgMusicInstance);
        }
    }
    //EG END 

    al_start_timer(timer);


    constellation = new Constellation(); // starImage / );
    constellation->generateAllStars();

    //player = new Player(); grid = new Grid();

    return true;
}


void Game::run() {
    running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.mouse.button == 1) { // Left click
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;

                for (StarTile* star : constellation->getStars()) {
                    int starX = star->getX();
                    int starY = star->getY();

                    // Assume star images are 32x32 px
                    if (mouseX >= starX && mouseX <= starX + 32 &&
                        mouseY >= starY && mouseY <= starY + 32) {

                        if (star->is_dead) {
                            // Stop music and show game over
                            al_stop_sample_instance(bgMusicInstance);
                            gameOver();
                        }
                        else if (star->is_unstable && !star->is_unlocked) {
                            // Stop music before starting mini-game
                            al_stop_sample_instance(bgMusicInstance);

                            // Create and run the mini-game
                            MiniGame miniGame;
                            miniGame.run();

                            // Properly restore Allegro components after mini-game
                            restoreAfterMiniGame();

                            // Check if player survived the mini-game
                            if (miniGame.survived()) {
                                // Player won the mini-game, unlock the star
                                constellation->unlockAfterMiniGame(star);

                                if (bgMusicInstance) {
                                    al_play_sample_instance(bgMusicInstance);
                                }

                                // Show a brief success message
                                showMessage("Star unlocked!", al_map_rgb(0, 255, 0), 1.0);
                            }
                            else {
                                // Player lost the mini-game
                                miniGameLost();
                            }
                        }
                        else {
                            // Normal star click
                            constellation->unlockStar(star);
                        }
                        break; // Found the clicked star, no need to check others
                    }
                }
            }
        }

        render();
    }
}



void Game::waitForSpaceToRestart() {
    ALLEGRO_FONT* font = al_load_ttf_font("Tagesschrift.ttf", 38, 0); 
    ALLEGRO_EVENT ev;

    while (true) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 +50, ALLEGRO_ALIGN_CENTER, "Press SPACE to restart");
        al_flip_display();

        al_wait_for_event(eventQueue, &ev);
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
            al_destroy_font(font);
            break;
        }
    }

    cleanup();     // Clean current game
    Game newGame;  // Create new game
    if (newGame.init()) {
        newGame.run();  // Restart game loop
    }
}


void Game::gameOver() {
    running = false;

    // Load explosion image and sound
    ALLEGRO_BITMAP* explosion = al_load_bitmap("explosion.png");
    ALLEGRO_SAMPLE* explosionSound = al_load_sample("Explosion.mp3");

    // Play explosion sound if loaded
    if (explosionSound) {
        std::cout << "Explosion sound loaded successfully.\n";  // Debugging
        al_play_sample(explosionSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    else {
        std::cerr << "Failed to load explosion sound!\n";
    }

    if (explosion) {
        int frameCount = 8;
        int frameWidth = 450; // Update frame width to match your sprite width
        int frameHeight = 274; // Update frame height to match your sprite height
        int centerX = (SCREEN_WIDTH - frameWidth) / 2; // Center horizontally
        int centerY = (SCREEN_HEIGHT - frameHeight) / 2; // Center vertically

        // Play explosion animation
        for (int i = 0; i < frameCount; ++i) {
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Clear the screen to black

            // Draw the current frame of the explosion animation
            al_draw_bitmap_region(
                explosion,
                i * frameWidth, 0,             // Source X, Y (for each frame)
                frameWidth, frameHeight,       // Source width & height
                centerX, centerY,              // Destination X, Y (centered)
                0                              // Flags
            );

            al_flip_display();
            al_rest(0.1); // Delay between frames

        }

        // Destroy explosion image after use
        al_destroy_bitmap(explosion);
    }

    // Show black screen with red "Game Over" message
    al_clear_to_color(al_map_rgb(0, 0, 0)); // Ensure screen is black before showing text

    /*ALLEGRO_FONT* font = al_create_builtin_font();
    if (font) {
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over");
        al_flip_display();
        al_rest(1); // Display message for 3 seconds
        al_destroy_font(font);
    }*/

    // Clean up explosion sound if loaded
    if (explosionSound) {
        al_destroy_sample(explosionSound);
    }
    waitForSpaceToRestart();
}



// New function for mini-game loss
void Game::miniGameLost() {
    running = false;

    // Show black screen with red "Game Over" message
    al_clear_to_color(al_map_rgb(0, 0, 0));

    ALLEGRO_FONT* font = al_create_builtin_font();
    if (font) {
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over");
        al_flip_display();
        al_rest(1); // Display message for 3 seconds
        al_destroy_font(font);
    }

    waitForSpaceToRestart();
}


void Game::render() {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (background) {
        al_draw_bitmap(background, 0, 0, 0);
    }

    if (constellation) {
        constellation->drawConstellation(mouseX, mouseY);
    }

    // Draw game objects here
    // e.g. grid->draw(); player->draw(); 


    al_flip_display();
}


void Game::cleanup() {
    if (background) al_destroy_bitmap(background);

    // Clean up audio resources
    if (MainMusic) al_destroy_sample(MainMusic);
    if (bgMusicInstance) al_destroy_sample_instance(bgMusicInstance);

    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
    if (eventQueue) al_destroy_event_queue(eventQueue);

    if (constellation) delete constellation;
    //if (starImage) al_destroy_bitmap(starImage);
}

//NEW FUNCTIONS
void Game::restoreAfterMiniGame() {
    // Reinitialize critical Allegro components
    al_set_target_backbuffer(display);

    // Reregister all event sources
    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    // Clear event queue to avoid processing old inputs
    al_flush_event_queue(eventQueue);

    // Restart the timer
    al_stop_timer(timer);
    al_start_timer(timer);

    // Note: We don't restart the main music here - that will happen 
    // conditionally based on whether the player won or lost the mini-game
}

void Game::showMessage(const char* text, ALLEGRO_COLOR color, float duration) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // Draw background
    if (background) {
        al_draw_bitmap(background, 0, 0, 0);
    }

    // Draw constellation
    if (constellation) {
        constellation->drawConstellation(mouseX, mouseY);
    }

    // Draw message
    ALLEGRO_FONT* font = al_create_builtin_font();
    if (font) {
        al_draw_text(font, color, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, text);
        al_flip_display();
        al_rest(duration);
        al_destroy_font(font);
    }
}