#include <iostream>
#include <string>
#include <glad/glad.h>
#include "shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Sprites/Sprite.h"
#include "Sprites/Player.h"
#include "Sprites/Fish.h"
#include "Sprites/Trash.h"
#include "Sprites/BackgroundLayer.h"
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "GameValues.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow * window, Player * player);

void error_callback(int error, const char *msg);

GLuint loadTexture(const std::string& texturePath);

void getBackgroundTexIds(int backgroundFolder, std::vector<GLuint> *texIdVector);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    srand((int)glfwGetTime());

    glfwSetErrorCallback(error_callback);

    int screenWidth = 1600;
    int screenHeight = 900;

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("../shaders/texture.vert", "../shaders/animatedTexture.frag");

    GLuint playerSlowTexId = loadTexture("../Textures/Characters/Shark/Walk.png");
    GLuint playerForwardTexId = loadTexture("../Textures/Characters/Shark/Attack.png");
    GLuint playerBackwardsTexId = loadTexture("../Textures/Characters/Shark/Idle.png");
    GLuint playerHurtTexId = loadTexture("../Textures/Characters/Shark/Hurt.png");
    GLuint fishTexId = loadTexture("../Textures/Fish/4.png");
    GLuint trashTexId = loadTexture("../Textures/Objects/trash.png");

    std::vector<GLuint> texIdVector;
    getBackgroundTexIds(1, &texIdVector);
    std::vector<BackgroundLayer*> bgLayers;
    int numberOfLayers = (int) texIdVector.size();
    for (int i = 0; i < numberOfLayers; ++i) {
        float step = (float) i * (playerMoveSpeed / (float) (numberOfLayers - 1));
        auto *bgLayer = new BackgroundLayer();
        bgLayer->setShader(&shader);
        bgLayer->initialize(texIdVector[i], screenWidth, screenHeight, step, 0.01);
        bgLayers.push_back(bgLayer);
    }

    Player player;
    player.setShader(&shader);
    player.initialize(playerSlowTexId, playerForwardTexId, playerBackwardsTexId, playerHurtTexId,
                      glm::vec3(200.0, 450.0, 0.0), glm::vec3(100.0, 100.0, 1.0), 0.0,
                      screenWidth, screenHeight, 1, 4, 0, 0.15, 0.01);

    std::vector<Fish*> fishes;
    for (int i = 0; i < 5; ++i) {
        auto *fish = new Fish();
        fish->setShader(&shader);
        fish->initialize(fishTexId, glm::vec3(52.0, 16.0, 1.0), 0.0, 3.0, screenWidth, screenHeight, 1, 2, 0, 0.15, 0.01);
        fishes.push_back(fish);
    }

    std::vector<Trash*> trashVector;
    for (int i = 0; i < 10; ++i) {
        auto *trash = new Trash();
        trash->setShader(&shader);
        trash->initialize(trashTexId, 0.0, screenWidth, screenHeight, 0.01);
        trashVector.push_back(trash);
    }

    int playerScore = 0;

    shader.use();

    glActiveTexture(GL_TEXTURE0);

    glm::mat4 projection = glm::ortho(0.0f,1600.0f,0.0f,900.0f,-1.0f,1.0f);
    shader.setMat4("projection", glm::value_ptr(projection));

    shader.setInt("texBuffer", 0);

    // game loop
    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        // input
        glfwPollEvents();
        processInput(window, &player);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto & bgLayer : bgLayers) {
            bgLayer->draw();
        }

        player.draw();

        for (auto & vectorFish : fishes) {
            if (player.collidesWith(vectorFish)) {
                vectorFish->resetPosition();
                playerScore++;
            }
            vectorFish->draw();
        }

        for (auto & trash : trashVector) {
            if (player.collidesWith(trash)) {
                player.setHurt();
            }
            trash->draw();
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window, Player * player) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    std::unordered_map<int,bool> wasdMap = {};
    wasdMap[GLFW_KEY_W] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    wasdMap[GLFW_KEY_A] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    wasdMap[GLFW_KEY_S] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    wasdMap[GLFW_KEY_D] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    player->move(5.0f, wasdMap);
}

void error_callback(int error, const char *msg) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

GLuint loadTexture(const std::string& texturePath)
{
    GLuint texId;

    // Gera o identificador da textura na memória
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    //Configuração do parâmetro WRAPPING nas coords s e t
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Confirugação do parâmetro FILTERING na minificação e magnificação da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) //jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else //png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texId;
}

void getBackgroundTexIds(int backgroundFolder, std::vector<GLuint> *texIdVector) {
    for (auto const& dir_entry : std::filesystem::directory_iterator
        {"../Textures/Backgrounds/" + std::to_string(backgroundFolder) + "_game_background/layers"}) {

        texIdVector->push_back(loadTexture(dir_entry.path().string()));
    }
}
