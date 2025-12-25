#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include<iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Player.h"

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;

const std::vector<std::string> bgVector = {
    "D:/Projects/hello_fighter/hello_fighter_2D_game/backgrounds/game_background_1/game_background_1.png",
    "D:/Projects/hello_fighter/hello_fighter_2D_game/backgrounds/game_background_2/game_background_2.png",
    "D:/Projects/hello_fighter/hello_fighter_2D_game/backgrounds/game_background_3/game_background_3.png",
    "D:/Projects/hello_fighter/hello_fighter_2D_game/backgrounds/game_background_4/game_background_4.png"
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* bgTexture = nullptr;

Player player1(200, 300, PlayerID::PLAYER_ONE);
Player player2(500, 300, PlayerID::PLAYER_TWO);

// Remove after test
SDL_Rect enemy_collider = { 200, 340, 48, 48 };


bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow(
        "Hello Fighter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) return false;

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) return false;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    player2.setDirection(false);

    return true;
}

bool loadBackground(int index)
{
    SDL_Surface* surface = IMG_Load(bgVector[index].c_str());
    if (!surface) return false;

    bgTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return bgTexture != nullptr;
}

int main(int argc, char* args[])
{
    if (!init()) {
        printf("Initialization failed\n");
        return 1;
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    int bgIndex = rand() % bgVector.size();

    if (!loadBackground(bgIndex)) {
        printf("Background load failed\n");
        return 1;
    }

    // LOAD PLAYER ANIMATIONS ONCE
    player1.addAnimation(
        renderer,
        "idle",
        "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Idle.png",
        6,
        12
    );
    player1.addAnimation(renderer, "run_r", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Run.png", 8, 8);
    player1.addAnimation(renderer, "run_l", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Run.png", 8, 8);
    player1.addAnimation(renderer, "jump", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Jump.png", 10, 10);
    player1.addAnimation(renderer, "shield", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Shield.png", 2, 1);
    player1.addAnimation(renderer, "attack1", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Attack_1.png", 4, 4);
    player1.addAnimation(renderer, "attack2", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Attack_2.png", 3, 3);
    player1.addAnimation(renderer, "attack3", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Attack_3.png", 4, 3);
    player1.addAnimation(renderer, "dead", "D:/Projects/hello_fighter/hello_fighter_2D_game/fighter/Dead.png", 3, 3);
    player1.setAnimation("idle");

    player2.addAnimation(
        renderer,
        "idle",
        "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Idle.png",
        6,
        12
    );
    player2.addAnimation(renderer, "run_r", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Run.png", 8, 8);
    player2.addAnimation(renderer, "run_l", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Run.png", 8, 8);
    player2.addAnimation(renderer, "jump", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Jump.png", 12, 12);
    player2.addAnimation(renderer, "shield", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Shield.png", 4, 2);
    player2.addAnimation(renderer, "attack1", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Attack_1.png", 5, 5);
    player2.addAnimation(renderer, "attack2", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Attack_2.png", 3, 3);
    player2.addAnimation(renderer, "attack3", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Attack_3.png", 4, 3);
    player2.addAnimation(renderer, "dead", "D:/Projects/hello_fighter/hello_fighter_2D_game/shinobi/Dead.png", 4, 3);
    player2.setAnimation("idle");

    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;

            player1.handleEvent(e);
            player2.handleEvent(e);
        }
        

        player1.update();
        player2.update();

        // Combat System: Check if attacks hit
        if (player1.getIsAttacking()) {
            if (SDL_HasIntersection(&player1.getAttackCollider(), &player2.getCollider())) {
                // Player 1 hits Player 2
                float damage = 5.0f; // Base damage
                player2.takeDamage(damage);
            }
        }

        if (player2.getIsAttacking()) {
            if (SDL_HasIntersection(&player2.getAttackCollider(), &player1.getCollider())) {
                // Player 2 hits Player 1
                float damage = 5.0f; // Base damage
                player1.takeDamage(damage);
            }
        }

        SDL_RenderClear(renderer);

        // Render background
        SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);

        // Render player
        player1.render(renderer);
        player2.render(renderer);

     //   SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
     //   SDL_RenderDrawRect(renderer, &player1.getCollider());

     //   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
     //   SDL_RenderDrawRect(renderer, &player2.getCollider());

        player1.renderHealthBar(renderer, SCREEN_WIDTH);
        player2.renderHealthBar(renderer, SCREEN_WIDTH);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    player1.clean();
    player2.clean();
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
