#include "Constellation.hpp"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FPS 60

Constellation::Constellation(/*ALLEGRO_BITMAP* sprite*/) {
    /*spriteStar = sprite;*/
}

Constellation::~Constellation() {

}

//void Constellation::generateAllStars(/*ALLEGRO_BITMAP* spriteStar, int totalStars*/) {
//    const int TILE_SIZE = 32;
//    const int ROWS = SCREEN_HEIGHT / TILE_SIZE;
//    const int COLS = SCREEN_WIDTH / TILE_SIZE;
//
//    stars.clear();
//
//    // Create grid
//    for (int row = 0; row < ROWS; ++row) {
//        for (int col = 0; col < COLS; ++col) {
//            int x = col * TILE_SIZE;
//            int y = row * TILE_SIZE;
//            StarTile* newStar = new StarTile(x, y);
//
//            // 1 in 6 chance of dead star (adjust as needed)
//            if (rand() % 6 == 0) {
//                newStar->markDead();
//            }
//
//            stars.push_back(newStar);  // Store in a flat vector
//        }
//    }
//
//    // Set adjacency for all stars -- logic ? distance ? DECIDE
//    for (int row = 0; row < ROWS; ++row) {
//        for (int col = 0; col < COLS; ++col) {
//            int index = row * COLS + col;
//            StarTile* current = stars[index];
//
//            for (int dRow = -1; dRow <= 1; ++dRow) {
//                for (int dCol = -1; dCol <= 1; ++dCol) {
//                    if (dRow == 0 && dCol == 0) continue; // skip self
//
//                    int nRow = row + dRow;
//                    int nCol = col + dCol;
//
//                    if (nRow >= 0 && nRow < ROWS && nCol >= 0 && nCol < COLS) {
//                        int neighborIndex = nRow * COLS + nCol;
//                        current->addAdjacent(stars[neighborIndex]);
//                    }
//                }
//            }
//
//            current->countAdjacentDead();  // update dead count for each star
//        }
//    }
//}

void Constellation::generateAllStars() {
    const int CELL_SIZE = 80;  // Larger square for more scattered stars
    const int ROWS = SCREEN_HEIGHT / CELL_SIZE;
    const int COLS = SCREEN_WIDTH / CELL_SIZE;

    stars.clear();
    std::vector<std::vector<StarTile*>> grid(ROWS, std::vector<StarTile*>(COLS, nullptr));

    // Step 1: Place stars randomly within each large cell
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int cellX = col * CELL_SIZE;
            int cellY = row * CELL_SIZE;

            // Random offset inside the cell
            int offsetX = rand() % (CELL_SIZE - 32);  // Ensure it doesn't exceed boundary
            int offsetY = rand() % (CELL_SIZE - 32);

            int starX = cellX + offsetX;
            int starY = cellY + offsetY;

            StarTile* newStar = new StarTile(starX, starY);

            // 1 in 6 chance it's a dead star
            int r = rand() % 6;  // Generates 0 to 5

            if (r == 0) {
                newStar->markDead();           // 1 in 6 chance to be dead
            }
            else if (r == 1) {
                newStar->is_unstable = true;   // 1 in 6 chance to be unstable
            }
            // else: it's an alive, normal star


            stars.push_back(newStar);
            grid[row][col] = newStar;  // Keep track for neighbor setup
        }
    }

    // Step 2: Set up neighbors
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            StarTile* current = grid[row][col];
            if (!current) continue;

            for (int dRow = -1; dRow <= 1; ++dRow) {
                for (int dCol = -1; dCol <= 1; ++dCol) {
                    if (dRow == 0 && dCol == 0) continue;

                    int nRow = row + dRow;
                    int nCol = col + dCol;

                    if (nRow >= 0 && nRow < ROWS && nCol >= 0 && nCol < COLS) {
                        StarTile* neighbor = grid[nRow][nCol];
                        if (neighbor) {
                            current->addAdjacent(neighbor);
                        }
                    }
                }
            }

            current->countAdjacentDead();
        }
    }
}


void Constellation::revealStar(StarTile* star) {
    if (star->is_dead || star->is_unlocked) return;

    star->unlock();
    revealedStars.push_back(star);


    if (star->countAdjacentDead() == 0) {
        for (StarTile* neighbor : star->adjacent_stars) {
            revealStar(neighbor);  // recursive reveal
        }
    }
}

//connecting to last unlocked star
//void Constellation::unlockStar(StarTile* clicked) {
//    if (!clicked->is_dead && !clicked->is_unlocked) {
//        clicked->unlock();
//        if (lastUnlocked != nullptr) {
//            lastUnlocked->nextStar = clicked;
//        }
//        lastUnlocked = clicked;
//    }
//}

void Constellation::unlockStar(StarTile* clicked) {
    if (!clicked->is_dead && !clicked->is_unlocked) {
        clicked->unlock();
        clicked->countAdjacentDead();

        //for (StarTile* neighbor : clicked->adjacent_stars) {
        //    if (neighbor->is_unlocked && !neighbor->is_dead) {
        //        // Add connection both ways (optional for bi-directional)
        //        clicked->nextStar = neighbor;  // You could maintain a list of connected stars instead
        //        break; // only connect to one for now; or remove break for multi-connect
        //    }
        //}
        if (lastUnlocked != nullptr) {
            lastUnlocked->nextStar = clicked;  // Connect previous to this
        }

        lastUnlocked = clicked;  // Update last
    }
}

int Constellation::countAdjacentDeadStars(StarTile* star) {
    int count = 0;
    for (StarTile* neighbor : star->adjacent_stars) {
        if (neighbor && neighbor->is_dead) {
            count++;
        }
    }
    return count;
}

//void Constellation::drawConstellation(int mouseX, int mouseY) {
//    for (StarTile* s : stars) {
//        al_draw_filled_circle(
//            s->getX() + 16, s->getY() + 16,
//            s->getRadius(),
//            al_map_rgb(211, 211, 211)
//    };
//
//            if (mouseX >= star->getX() && mouseX <= star->getX() + 32 && mouseY >= star->getY() && mouseY <= star->getY() + 32 && !star->is_dead) {
//
//                int count = countAdjacentDeadStars(star); //  you'll write this function
//                ALLEGRO_FONT* font = al_create_builtin_font();
//                if (font) {
//                    al_draw_textf(font, al_map_rgb(255, 255, 255),
//                        star->getX() + 16, star->getY() - 20,
//                        ALLEGRO_ALIGN_CENTRE, "%d", count);
//                    al_destroy_font(font);
//                }
//            }
//      
//
//        // Draw line to nextStar if set
//        if (s->nextStar != nullptr && s->is_unlocked) {
//            al_draw_line(
//                s->getX() + 16, s->getY() + 16,
//                s->nextStar->getX() + 16, s->nextStar->getY() + 16,
//                al_map_rgb(211, 211, 211), 1.0
//            );
//        }
//    
//}

void Constellation::unlockAfterMiniGame(StarTile* unstableStar) {
    if (unstableStar && unstableStar->is_unstable && !unstableStar->is_unlocked) {
        unstableStar->is_unstable = false;    // It's no longer unstable
        unstableStar->unlock();               // Now treated like a regular unlocked star
        unstableStar->countAdjacentDead();    // Update number of adjacent dead stars

        if (lastUnlocked != nullptr) {
            lastUnlocked->nextStar = unstableStar;
        }

        lastUnlocked = unstableStar;          // Update trail
    }
}

void Constellation::drawConstellation(int mouseX, int mouseY) {
    for (StarTile* s : stars) {
        // Draw the star
        al_draw_filled_circle(
            s->getX() + 16, s->getY() + 16,
            s->getRadius(),
            al_map_rgb(211, 211, 211)
        );

        // Check if mouse is over the star and it's not dead
        if (mouseX >= s->getX() && mouseX <= s->getX() + 32 &&
            mouseY >= s->getY() && mouseY <= s->getY() + 32 &&
            !s->is_dead && s->is_unlocked) {  // Only if it's revealed and not dead

            int count = countAdjacentDeadStars(s);
            std::cout << "Mouse over revealed star, count: " << count << std::endl;

            ALLEGRO_FONT* font = al_create_builtin_font();
            if (font) {
                al_draw_textf(font, al_map_rgb(255, 255, 255),
                    s->getX() + 16, s->getY() - 40,
                    ALLEGRO_ALIGN_CENTRE, "%d", count);
                al_destroy_font(font);
            }
        }



        // Draw line to nextStar if set and star is unlocked
        if (s->nextStar != nullptr && s->is_unlocked) {
            al_draw_line(
                s->getX() + 16, s->getY() + 16,
                s->nextStar->getX() + 16, s->nextStar->getY() + 16,
                al_map_rgb(211, 211, 211), 1.0
            );
        }
    }
}


//255, 255, 255



float Constellation::distance(StarTile* a, StarTile* b) {
    float dx = a->getX() - b->getX();
    float dy = a->getY() - b->getY();
    return sqrt(dx * dx + dy * dy);
}