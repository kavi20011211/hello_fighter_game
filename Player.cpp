#include "Player.h"

const int SCREEN_WIDTH = 900;

Player::Player(float startX, float startY, PlayerID id)
    : x(startX), y(startY), isMoving(false), isJumping(false),
    isGrounded(true), groundY(300), health(100), playerID(id) {

    if (playerID == PlayerID::PLAYER_ONE) {
        keys.moveLeft = SDL_SCANCODE_LEFT;
        keys.moveRight = SDL_SCANCODE_RIGHT;
        keys.jump = SDL_SCANCODE_UP;
        keys.shield = SDL_SCANCODE_DOWN;
        keys.attack1 = SDL_SCANCODE_A;
        keys.attack2 = SDL_SCANCODE_S;
        keys.attack3 = SDL_SCANCODE_D;
    }
    else {
        keys.moveLeft = SDL_SCANCODE_A;
        keys.moveRight = SDL_SCANCODE_D;
        keys.jump = SDL_SCANCODE_W;
        keys.shield = SDL_SCANCODE_J;
        keys.attack1 = SDL_SCANCODE_U;
        keys.attack2 = SDL_SCANCODE_I;
        keys.attack3 = SDL_SCANCODE_O;
    }
}

void Player::handleEvent(const SDL_Event& e)
{
    if (isDead) return;

    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.scancode == keys.moveRight) {
            isMoving = true;
            isSafe = false;
            setAnimation("run_r");
            facingRight = true;
        }
        else if (e.key.keysym.scancode == keys.moveLeft) {
            isMoving = true;
            isSafe = false;
            setAnimation("run_l");
            facingRight = false;
        }
        else if (e.key.keysym.scancode == keys.jump && isGrounded) {
            velocityY = jumpStrength;
            isJumping = true;
            isGrounded = false;
            isSafe = false;
            setAnimation("jump");
        }
        else if (e.key.keysym.scancode == keys.shield) {
            isMoving = false;
            isSafe = true;
            isAttacking = false;
            setAnimation("shield");
        }
        else if (e.key.keysym.scancode == keys.attack1) {
            isMoving = false;
            isSafe = false;
            isAttacking = true;
            currentAttackType = "attack1";
            currentAttackFrame = 0;
            setAnimation("attack1");
        }
        else if (e.key.keysym.scancode == keys.attack2) {
            isMoving = false;
            isSafe = false;
            isAttacking = true;
            currentAttackType = "attack2";
            currentAttackFrame = 0;
            setAnimation("attack2");
        }
        else if (e.key.keysym.scancode == keys.attack3) {
            isMoving = false;
            isSafe = false;
            isAttacking = true;
            currentAttackType = "attack3";
            currentAttackFrame = 0;
            setAnimation("attack3");
        }
    }
    else if (e.type == SDL_KEYUP)
    {
        if (e.key.keysym.scancode == keys.moveRight ||
            e.key.keysym.scancode == keys.moveLeft) {
            isMoving = false;
        }
        else if (e.key.keysym.scancode == keys.shield) {
            isSafe = false;
        }
    }
}

void Player::addAnimation(
    SDL_Renderer* renderer,
    const std::string& name,
    const std::string& path,
    int frames,
    int delay
) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return;

    Animation anim;
    anim.texture = SDL_CreateTextureFromSurface(renderer, surface);
    anim.totalFrames = frames;
    anim.frameDelay = delay;
    anim.frameWidth = surface->w / frames;
    anim.frameHeight = surface->h;

    animations[name] = anim;
    SDL_FreeSurface(surface);

    if (!currentAnimation)
        currentAnimation = &animations[name];
}

void Player::setAnimation(const std::string& name) {
    if (isDead && name != "dead") return;

    if (animations.find(name) != animations.end() &&
        &animations[name] != currentAnimation) {
        currentAnimation = &animations[name];
        currentFrame = 0;
        frameTimer = 0;
    }
}

void Player::update() {
    if (!currentAnimation) return;

    if (health <= 0.0f && !isDead) {
        isDead = true;
        setAnimation("dead");
        velocityY = 0;
        isMoving = false;
        isJumping = false;
        isAttacking = false;
        return;
    }

    if (isDead) {
        frameTimer++;
        if (frameTimer >= currentAnimation->frameDelay) {
            frameTimer = 0;
            if (currentFrame < currentAnimation->totalFrames - 1) {
                currentFrame++;
            }
        }
        return;
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (!isAttacking) {
        if (keystate[keys.moveRight]) {
            x += 3;
            if (x > SCREEN_WIDTH - currentAnimation->frameWidth) {
                x = SCREEN_WIDTH - currentAnimation->frameWidth;
            }
            if (!isMoving) {
                isMoving = true;
                setAnimation("run_r");
                facingRight = true;
            }
        }
        else if (keystate[keys.moveLeft]) {
            x -= 3;
            if (x < 0) x = 0;
            if (!isMoving) {
                isMoving = true;
                setAnimation("run_l");
                facingRight = false;
            }
        }
    }

    if (!isGrounded) {
        velocityY += gravity;
        y += velocityY;

        if (y >= groundY) {
            y = groundY;
            velocityY = 0;
            isGrounded = true;
            isJumping = false;
        }
    }

    // Check for shield
    if (keystate[keys.shield] && !isMoving && !isAttacking) {
        isSafe = true;
        setAnimation("shield");
    }
    else if (!isAttacking && !keystate[keys.moveRight] && !keystate[keys.moveLeft] &&
        !keystate[keys.shield] && isGrounded) {
        if (isMoving || currentAnimation != &animations["idle"]) {
            isMoving = false;
            isSafe = false;
            setAnimation("idle");
        }
    }

    // Update animation frame
    frameTimer++;
    if (frameTimer >= currentAnimation->frameDelay) {
        frameTimer = 0;
        currentFrame++;

        // Check if attack animation finished
        if (isAttacking && currentFrame >= currentAnimation->totalFrames) {
            isAttacking = false;
            currentAttackType = "";
            currentFrame = 0;
            setAnimation("idle");
        }
        else if (currentFrame >= currentAnimation->totalFrames) {
            currentFrame = 0;
        }
    }

    // Update collider position
    player_collider.x = static_cast<int>(x + currentAnimation->frameWidth * 0.4f);
    player_collider.y = static_cast<int>(y + currentAnimation->frameHeight * 0.4f);
    player_collider.w = static_cast<int>(currentAnimation->frameWidth * 0.2f);
    player_collider.h = static_cast<int>(currentAnimation->frameHeight * 0.5f);

    // Update attack collider
    if (isAttacking) {
        int attackRange = 80;
        int attackWidth = 60;
        int attackHeight = 80;

        if (facingRight) {
            attack_collider.x = static_cast<int>(x + currentAnimation->frameWidth * 0.5f);
            attack_collider.y = static_cast<int>(y + currentAnimation->frameHeight * 0.3f);
        }
        else {
            attack_collider.x = static_cast<int>(x + currentAnimation->frameWidth * 0.5f - attackRange);
            attack_collider.y = static_cast<int>(y + currentAnimation->frameHeight * 0.3f);
        }
        attack_collider.w = attackWidth;
        attack_collider.h = attackHeight;
    }
    else {
        attack_collider = { 0, 0, 0, 0 };
    }
}

void Player::render(SDL_Renderer* renderer) {
    if (!currentAnimation) return;

    SDL_Rect src{
        currentFrame * currentAnimation->frameWidth,
        0,
        currentAnimation->frameWidth,
        currentAnimation->frameHeight
    };

    SDL_Rect dst{
        static_cast<int>(x),
        static_cast<int>(y),
        currentAnimation->frameWidth,
        currentAnimation->frameHeight
    };

    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, currentAnimation->texture, &src, &dst, 0, nullptr, flip);
}

void Player::renderHealthBar(SDL_Renderer* renderer, int screenWidth) {
    int barWidth = 300;
    int barHeight = 30;
    int barX, barY = 20;

    // Position health bar based on player
    if (playerID == PlayerID::PLAYER_ONE) {
        barX = 20;
    }
    else {
        barX = screenWidth - barWidth - 20;
    }

    // Background (red)
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_Rect bgRect = { barX, barY, barWidth, barHeight };
    SDL_RenderFillRect(renderer, &bgRect);

    // Health (green)
    float healthPercent = health / maxHealth;
    int healthWidth = static_cast<int>(barWidth * healthPercent);
    SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
    SDL_Rect healthRect = { barX, barY, healthWidth, barHeight };
    SDL_RenderFillRect(renderer, &healthRect);

    // Border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bgRect);
}

void Player::clean() {
    for (auto& pair : animations) {
        SDL_DestroyTexture(pair.second.texture);
    }
}