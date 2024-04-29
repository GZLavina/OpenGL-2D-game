
#include "Trash.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../GameValues.h"

Trash::~Trash() {
    glDeleteVertexArrays(1, &VAO);
}

void Trash::initialize(GLuint texId_, float angle_, int screenWidth_, int screenHeight_, double movementInterval_, std::vector<GLuint> *texIds_) {
    glm::vec3 scale_ = glm::vec3(32.0, 32.0, 1.0);
    this->texId = texId_;
    this->scale = scale_;
    this->angle =(float) (rand() % 360);
    this->screenWidth = screenWidth_;
    this->screenHeight = screenHeight_;
    this->width = scale_.x;
    this->height = scale_.y;
    this->spriteRows = 1;
    this->spriteColumns = 1;
    this->currentRow = 0;
    this->lastAnimationUpdateTime = glfwGetTime();
    this->lastMovementUpdateTime = glfwGetTime();
    this->animationInterval = 1;
    this->movementInterval = movementInterval_;
    this->currentRotationDirection = rand() % 3;
    this->texIds = texIds_;
    this->reset();

    this->setVAO();
}

void Trash::reset() {
    // reset position
    this->pos.x = (float) (this->screenWidth + (rand() % (this->screenWidth * 2)));
    this->pos.y = (float) (this->screenHeight + 100 + (rand() % 400));

    // select new color
    int index = rand() % this->texIds->size();
    this->texId = this->texIds->at(index);
}

void Trash::update() {
    if (this->pos.x < -100.0 || this->pos.y < -100.0) {
        this->reset();
    }

    if (glfwGetTime() - this->lastMovementUpdateTime > this->movementInterval) {
        this->lastMovementUpdateTime = glfwGetTime();
        this->pos.x -= playerMoveSpeed;
        this->pos.y -= 1.0f;
        this->rotate();
    }

    Sprite::update();
}

void Trash::rotate() {
    // small (2/90) chance to change direction - makes it float more realistically
    int drawnDirection = rand() % 90;
    if (drawnDirection < 3 && drawnDirection != this->currentRotationDirection) {
        this->currentRotationDirection = drawnDirection;
    }

    if (this->currentRotationDirection == 0) {
        this->angle += 0.4f;
    } else if (this->currentRotationDirection == 2) {
        this->angle -= 0.4f;
    }
}


