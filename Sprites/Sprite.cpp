#include "Sprite.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>

Sprite::~Sprite() {
    glDeleteVertexArrays(1, &VAO);
}

void Sprite::initialize(GLuint texId_, glm::vec3 pos_, glm::vec3 scale_, float angle_,
                        int screenWidth_, int screenHeight_, int rows, int columns, int startingRow,
                        double animationInterval_, double movementInterval_) {
    this->texId = texId_;
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

    this->setVAO();
}

void Sprite::draw() {
    this->update();

    glBindTexture(GL_TEXTURE_2D, texId);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Sprite::update() {
    this->updateAnimation();
    this->applyModelTransformations();
}

void Sprite::setVAO() {
    // Calculate texture coordinates
    float dx = 1.0f / (float) this->spriteColumns;
    float dy = 1.0f / (float) this->spriteRows;

    GLfloat vertices[] = {
            //x     y    z    r    g    b    s    t
            //Triangulo 0
            -0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, dy,  // top-left
            -0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,  // bottom-left
             0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, dx, dy,  // top-right
            //Triangulo 1
            -0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,  // bottom-left
             0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, dx, 0.0,  // bottom-right
             0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, dx, dy   // top-right

    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Atributo 0 - posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Atributo 1 - cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //Atributo 2 - coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

float Sprite::getMaxX() const {
    return this->pos.x + (this->width/2);
}

float Sprite::getMinX() const {
    return this->pos.x - (this->width/2);
}

float Sprite::getMaxY() const {
    return this->pos.y + (this->height/2);
}

float Sprite::getMinY() const {
    return this->pos.y - (this->height/2);
}

bool Sprite::collidesWith(Sprite *that) {
    return (this->getMaxX() >= that->getMinX() && that->getMaxX() >= this->getMinX()) &&
           (this->getMaxY() >= that->getMinY() && that->getMaxY() >= this->getMinY());
}

void Sprite::updateAnimation() {
    if (this->spriteColumns > 1){
        float dx = 1.0f / (float) this->spriteColumns;
        float dy = 1.0f / (float) this->spriteRows;
        if (glfwGetTime() - this->lastAnimationUpdateTime >= this->animationInterval) {
            this->lastAnimationUpdateTime = glfwGetTime();
            this->currentColumn = (currentColumn + 1) % this->spriteColumns;
        }

        this->shader->setVec2("offset", (float) this->currentColumn * dx, (float) this->currentRow * dy);

    } else {
        this->shader->setVec2("offset", 0.0f, 0.0f);
    }
}

void Sprite::applyModelTransformations() {
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, scale);
    this->shader->setMat4("model", glm::value_ptr(model));
}

void Sprite::changeTexture(GLuint newTexId, int rows, int columns, int startingRow, float newAnimationInterval) {
    this->texId = newTexId;
    this->spriteRows = rows;
    this->spriteColumns = columns;
    this->currentRow = startingRow;
    if (this->currentColumn >= columns) {
        this->currentColumn = 0;
    }
    this->animationInterval = newAnimationInterval;
    this->setVAO();
}
