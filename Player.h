#pragma once
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

struct Animation {
    SDL_Texture* texture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    int totalFrames = 0;
    int frameDelay = 0;
};

enum class PlayerID {
    PLAYER_ONE,
    PLAYER_TWO
};

class Player {
private:
    float x, y;
    float velocityY = 0;
    float gravity = 0.8f;
    float jumpStrength = -15.0f;
    float groundY;
    float health;
    float maxHealth = 100.0f;
    bool isDead = false;
    SDL_Rect player_collider;
    SDL_Rect attack_collider;
    Uint32 lastHitTime = 0;
    Uint32 hitCooldown = 500;
    bool isSafe = true;
    bool isAttacking = false;
    int currentAttackFrame = 0;
    std::string currentAttackType = "";

    std::map<std::string, Animation> animations;
    Animation* currentAnimation = nullptr;
    int currentFrame = 0;
    int frameTimer = 0;
    bool facingRight = true;
    bool isMoving = false;
    bool isJumping = false;
    bool isGrounded = true;

    PlayerID playerID;

    struct KeyBindings {
        SDL_Scancode moveLeft;
        SDL_Scancode moveRight;
        SDL_Scancode jump;
        SDL_Scancode shield;
        SDL_Scancode attack1;
        SDL_Scancode attack2;
        SDL_Scancode attack3;
    };
    KeyBindings keys;

public:
    Player(float startX, float startY, PlayerID id);

    void handleEvent(const SDL_Event& e);
    void addAnimation(
        SDL_Renderer* renderer,
        const std::string& name,
        const std::string& path,
        int frames,
        int delay
    );

    const SDL_Rect& getCollider() const {
        return player_collider;
    }

    const SDL_Rect& getAttackCollider() const {
        return attack_collider;
    }

    float getHealth() const {
        return health;
    }

    float getMaxHealth() const {
        return maxHealth;
    }

    bool getIsDead() const {
        return isDead;
    }

    bool getIsAttacking() const {
        return isAttacking;
    }

    bool getIsSafe() const {
        return isSafe;
    }

    float getX() const { return x; }
    float getY() const { return y; }

    PlayerID getPlayerID() const {
        return playerID;
    }

    void takeDamage(float damage) {
        if (isSafe || isDead) return;

        Uint32 now = SDL_GetTicks();
        if (now - lastHitTime >= hitCooldown) {
            health -= damage;
            if (health < 0.0f) health = 0.0f;
            lastHitTime = now;
        }
    }

    void setDirection(bool isTurn) {
        if (facingRight) {
            facingRight = isTurn;
        }
    }

    void setAnimation(const std::string& name);
    void update();
    void render(SDL_Renderer* renderer);
    void renderHealthBar(SDL_Renderer* renderer, int screenWidth);
    void clean();
};