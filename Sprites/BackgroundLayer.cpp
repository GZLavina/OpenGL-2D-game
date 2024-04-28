
#include "BackgroundLayer.h"
#include "GLFW/glfw3.h"

void BackgroundLayer::initialize(
        GLuint texId_, int screenWidth_, int screenHeight_, float step_, float movementInterval_) {

    glm::vec3 pos_ = glm::vec3((float) screenWidth_ / 2.0f, (float) screenHeight_ / 2.0f, 1.0f);
    glm::vec3 scale_ = glm::vec3((float) screenWidth_ * 3, (float) screenHeight_, 1.0f);
    this->texId = texId_;
    this->pos = pos_;
    this->scale = scale_;
    this->angle = 0.0f;
    this->screenWidth = screenWidth_;
    this->screenHeight = screenHeight_;
    this->width = scale_.x;
    this->height = scale_.y;
    this->spriteRows = 1;
    this->spriteColumns = 1;
    this->currentRow = 0;
    this->currentColumn = 0;
    this->lastAnimationUpdateTime = glfwGetTime();
    this->lastMovementUpdateTime = glfwGetTime();
    this->animationInterval = 1;
    this->movementInterval = movementInterval_;
    this->step = step_;

    this->setVAO();
}

BackgroundLayer::~BackgroundLayer() {
    glDeleteVertexArrays(1, &VAO);
}

void BackgroundLayer::update() {
//    std::cout << "atualizando camada de texId " + std::to_string(this->texId) << std::endl;
//    std::cout << pos.x << std::endl;
//    std::cout << pos.y << std::endl;
//    std::cout << scale.x << std::endl;
//    std::cout << scale.y << std::endl;

    if (this->pos.x <= ((float) this->screenWidth/2.0f) - (float) this->screenWidth) {
        this->pos.x = (float) this->screenWidth/2.0f;
    }

    if (glfwGetTime() - this->lastMovementUpdateTime > this->movementInterval) {
        this->lastMovementUpdateTime = glfwGetTime();
        this->pos.x -= step;
    }

    Sprite::update();
}

void BackgroundLayer::setVAO() {
    GLfloat vertices[] = {
            //x     y    z    r    g    b    s    t
            //Triangulo 0
            -0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,  // top-left
            -0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,  // bottom-left
            0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, 3.0, 1.0,  // top-right
            //Triangulo 1
            -0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,  // bottom-left
            0.5 ,-0.5, 0.0, 1.0, 0.0, 0.0, 3.0, 0.0,  // bottom-right
            0.5 , 0.5, 0.0, 1.0, 0.0, 0.0, 3.0, 1.0   // top-right
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
