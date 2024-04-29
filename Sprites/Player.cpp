
#include "Player.h"
#include <GLFW/glfw3.h>

void Player::initialize(GLuint slowTexId_, GLuint forwardTexId_, GLuint backwardTexId_, GLuint hurtTexId_, glm::vec3 pos_,
                        glm::vec3 scale_, float angle_, int screenWidth_, int screenHeight_, int rows, int columns,
                        int startingRow, double animationInterval_, double movementInterval_) {

    this->texId = slowTexId_;
    this->slowTexId = slowTexId_;
    this->forwardTexId = forwardTexId_;
    this->backwardTexId = backwardTexId_;
    this->hurtTexId = hurtTexId_;
    this->pos = pos_;
    this->scale = scale_;
    this->angle = angle_;
    this->screenWidth = screenWidth_;
    this->screenHeight = screenHeight_;
    this->width = scale_.x;
    this->height = scale_.y;
    this->spriteRows = rows;
    this->spriteColumns = columns;
    this->currentRow = startingRow;
    this->currentColumn = 0;
    this->lastAnimationUpdateTime = glfwGetTime();
    this->lastMovementUpdateTime = glfwGetTime();
    this->animationInterval = animationInterval_;
    this->movementInterval = movementInterval_;
    this->lastHurtTime = glfwGetTime();
    this->hurtInterval = 1;
    this->cannotGetHurtInterval = 1.5;
    this->isHurt = false;
    this->lives = 2;

    this->setVAO();
}

void Player::update() {
    if (isHurt) {
        if (glfwGetTime() - this->lastHurtTime < this->hurtInterval) {
            if (glfwGetTime() - this->lastMovementUpdateTime >= this->movementInterval) {
                this->lastMovementUpdateTime = glfwGetTime();
                this->moveLeft(0.5f);
            }
        } else {
            this->isHurt = false;
            this->setSlow();
        }
    }

    Sprite::update();
}

void Player::moveRight(float distance) {
    if (this->pos.x + (this->width/2) < (float) this->screenWidth) {
        this->pos.x += distance;
    }
}

void Player::moveLeft(float distance) {
    if (this->pos.x - (this->width/2) > 0) {
        this->pos.x -= distance;
    }
}

void Player::moveUp(float distance) {
    if (this->pos.y + (this->height/2) < (float) this->screenHeight) {
        this->pos.y += distance;
    }
}

void Player::moveDown(float distance) {
    if (this->pos.y - (this->height/2) > 0) {
        this->pos.y -= distance;
    }
}

void Player::move(float distance, std::unordered_map<int, bool>& wasdMap) {
    if (glfwGetTime() - this->lastMovementUpdateTime > this->movementInterval && !isHurt) {
        this->lastMovementUpdateTime = glfwGetTime();
        // Changes when keys pressed
        if (wasdMap[GLFW_KEY_W]) {
            this->moveUp(distance);
            this->angle = 15.0f;
        }
        if (wasdMap[GLFW_KEY_A]) {
            this->moveLeft(distance);
            this->setBackward();
        }
        if (wasdMap[GLFW_KEY_S]) {
            this->moveDown(distance);
            if (wasdMap[GLFW_KEY_W]) {
                this->angle = 0.0f;
            } else {
                this->angle = -15.0f;
            }
        }
        if (wasdMap[GLFW_KEY_D]) {
            this->moveRight(distance);
            if (wasdMap[GLFW_KEY_A]) {
                this->setSlow();
            } else {
                this->setForward();
            }
        }

        // Changes when keys released
        if (!wasdMap[GLFW_KEY_W] && !wasdMap[GLFW_KEY_S]) {
            this->angle = 0.0f;
        }
        if (!wasdMap[GLFW_KEY_A] && !wasdMap[GLFW_KEY_D]) {
            this->setSlow();
        }
    }
}

void Player::setSlow() {
    this->changeTexture(this->slowTexId, 1, 4, 0, 0.15);
}

void Player::setForward() {
    this->changeTexture(this->forwardTexId, 1, 6, 0, 0.10);
}

void Player::setBackward() {
    this->changeTexture(this->backwardTexId, 1, 4, 0, 0.15);
}

void Player::setHurt() {
    // Checks if player was just hurt - prevents consecutive damage to same obstacle
    if (!isHurt && glfwGetTime() - this->lastHurtTime > this->cannotGetHurtInterval) {
        this->isHurt = true;
        this->changeTexture(this->hurtTexId, 1, 2, 0, 0.15);
        this->lastHurtTime = glfwGetTime();
        this->lives -= 1;
    }
}

int Player::getLives() const {
    return this->lives;
}

