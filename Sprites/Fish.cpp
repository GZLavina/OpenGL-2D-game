
#include "Fish.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <random>
#include <GLFW/glfw3.h>

void Fish::update() {
    if (this->pos.x < -100.0) {
        this->reset();
    }

    if (glfwGetTime() - this->lastMovementUpdateTime > this->movementInterval) {
        this->lastMovementUpdateTime = glfwGetTime();
        this->pos.x -= step;
    }

    Sprite::update();
}

void Fish::initialize(GLuint texId_, glm::vec3 scale_, float angle_, float step_, int screenWidth_, int screenHeight_,
                      int rows, int columns, int startingRow,
                      double animationInterval_, double movementInterval_, std::unordered_map<GLuint,glm::vec3> *texIdToScaleMap_) {
    this->texId = texId_;
    this->scale = scale_;
    this->angle = angle_;
    this->step = step_;
    this->screenWidth = screenWidth_;
    this->screenHeight = screenHeight_;
    this->width = scale_.x;
    this->height = scale_.y;
    this->spriteRows = rows;
    this->spriteColumns = columns;
    this->currentRow = startingRow;
    this->lastAnimationUpdateTime = glfwGetTime();
    this->lastMovementUpdateTime = glfwGetTime();
    this->animationInterval = animationInterval_;
    this->movementInterval = movementInterval_;
    this->texIdToScaleMap = texIdToScaleMap_;
    this->reset();

    this->setVAO();
}

void Fish::reset() {
    // reset position
    this->pos.x = (float) (this->screenWidth + 100 + (rand() % 600));
    this->pos.y = (float) (100 + (rand() % (this->screenHeight - 300)));

    std::vector<GLuint> texIds;
    for (auto kv : *texIdToScaleMap) {
        texIds.push_back(kv.first);
    }
    this->texId = texIds[rand() % texIds.size()];
    this->scale = this->texIdToScaleMap->at(this->texId);
    this->width = this->scale.x;
    this->height = this->scale.y;
    this->setVAO();
}

Fish::~Fish() {
    glDeleteVertexArrays(1, &VAO);
}
