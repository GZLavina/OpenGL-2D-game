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
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "TextRenderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow * window, Player * player);

void error_callback(int error, const char *msg);

GLuint loadTexture(const std::string& texturePath);

void getBackgroundTexIds(int backgroundFolder, std::vector<GLuint> *texIdVector);

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, GLuint VAO, GLuint VBO);

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

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
    Shader textShader("../shaders/text.vert", "../shaders/text.frag");

    GLuint playerSlowTexId = loadTexture("../Textures/Characters/Shark/Walk4xCropped.png");
    GLuint playerForwardTexId = loadTexture("../Textures/Characters/Shark/Attack4xCropped.png");
    GLuint playerBackwardsTexId = loadTexture("../Textures/Characters/Shark/Idle4xCropped.png");
    GLuint playerHurtTexId = loadTexture("../Textures/Characters/Shark/Hurt4xCropped.png");

    GLuint redTrashTexId = loadTexture("../Textures/Objects/RedTinCan.png");
    GLuint greenTrashTexId = loadTexture("../Textures/Objects/GreenTinCan.png");
    GLuint blueTrashTexId = loadTexture("../Textures/Objects/BlueTinCan.png");
    GLuint purpleTrashTexId = loadTexture("../Textures/Objects/PurpleTinCan.png");
    // store different textures in a vector
    // passed as pointer to instances of Trash to pick a new texture when respawning
    std::vector<GLuint> trashTexIds = {redTrashTexId, greenTrashTexId, blueTrashTexId, purpleTrashTexId};

    GLuint blueFishTexId = loadTexture("../Textures/Fish/BlueFish4xCropped.png");
    GLuint brownFishTexId = loadTexture("../Textures/Fish/BrownFish4xCroppedCentered.png");
    GLuint greenFishTexId = loadTexture("../Textures/Fish/GreenFish4xCropped.png");
    GLuint uglyFishTexId = loadTexture("../Textures/Fish/UglyFish4xCropped.png");
    // same as with trash textures but different sizes require different scales,
    // so we keep scale vector in a map
    std::unordered_map<GLuint,glm::vec3> fishTexIdToScaleMap = {
            {blueFishTexId, glm::vec3(80.0f, 34.5f, 1.0f)},
            {brownFishTexId, glm::vec3(80.0f, 49.2f, 1.0f)},
            {greenFishTexId, glm::vec3(80.0f, 45.0f, 1.0f)},
            {uglyFishTexId, glm::vec3(100.0f, 40.68f, 1.0f)},
    };

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
                      glm::vec3(200.0, 450.0, 0.0), glm::vec3(160.0, 80.0, 1.0), 0.0,
                      screenWidth, screenHeight, 1, 4, 0, 0.15, 0.01);

    std::vector<Fish*> fishes;
    for (int i = 0; i < 5; ++i) {
        auto *fish = new Fish();
        fish->setShader(&shader);
        fish->initialize(blueFishTexId, glm::vec3(80.0f, 34.5f, 1.0f), 0.0, 2.0, screenWidth, screenHeight, 1, 2, 0, 0.15, 0.01, &fishTexIdToScaleMap);
        fishes.push_back(fish);
    }

    std::vector<Trash*> trashVector;
    for (int i = 0; i < 15; ++i) {
        auto *trash = new Trash();
        trash->setShader(&shader);
        trash->initialize(redTrashTexId, 0.0, screenWidth, screenHeight, 0.01, &trashTexIds);
        trashVector.push_back(trash);
    }

    TextRenderer textRenderer(textShader);
    textRenderer.load("../Fonts/MAIAN.TTF", 56);

//    // Loading text
//    FT_Library ft;
//    if (FT_Init_FreeType(&ft))
//    {
//        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
//        return -1;
//    }
//    FT_Face face;
//    if (FT_New_Face(ft, "../Fonts/arial.ttf", 0, &face))
//    {
//        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
//        return -1;
//    }
//    FT_Set_Pixel_Sizes(face, 0, 48);
//    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
//    {
//        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//        return -1;
//    }
//
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
//
//    for (unsigned char c = 0; c < 128; c++)
//    {
//        // load character glyph
//        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//        {
//            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//            continue;
//        }
//        // generate texture
//        unsigned int texture;
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glTexImage2D(
//                GL_TEXTURE_2D,
//                0,
//                GL_RED,
//                face->glyph->bitmap.width,
//                face->glyph->bitmap.rows,
//                0,
//                GL_RED,
//                GL_UNSIGNED_BYTE,
//                face->glyph->bitmap.buffer
//        );
//        // set texture options
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        // now store character for later use
//        Character character = {
//                texture,
//                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//                static_cast<unsigned int>(face->glyph->advance.x)
//        };
//        Characters.insert(std::pair<char, Character>(c, character));
//    }
//
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//    FT_Done_Face(face);
//    FT_Done_FreeType(ft);
//
//    unsigned int VAO, VBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

    // player score does not need to be calculated by the player object
    int playerScore = 0;

    glActiveTexture(GL_TEXTURE0);

    glm::mat4 projection = glm::ortho(0.0f,1600.0f,0.0f,900.0f,-1.0f,1.0f);

    shader.use();
    shader.setMat4("projection", glm::value_ptr(projection));
    shader.setInt("texBuffer", 0);

    textShader.use();
    textShader.setMat4("projection", glm::value_ptr(projection));
    textShader.setInt("texBuffer", 0);

    // game loop
    while(!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        // input
        glfwPollEvents();
        processInput(window, &player);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        // Draw all except for the closest to the screen,
        // which looks better if it appears on top of other elements
        for (int i = 0; i < bgLayers.size() - 1; ++i) {
            bgLayers[i]->draw();
        }

        player.draw();

        for (auto & vectorFish : fishes) {
            if (player.collidesWith(vectorFish)) {
                vectorFish->reset();
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

        //draw last layer
        bgLayers.back()->draw();

        //Draw text
        textShader.use();

        if (player.getLives() <= 0) {
            textRenderer.renderText("GAME OVER", ((float) screenWidth / 2.0f) - ((float) screenWidth * 0.10f), (float) screenHeight / 2.0f, 1.0, glm::vec3(0.0f, 0.0f, 0.0f));
        }

        textRenderer.renderText(std::to_string(player.getLives()), 0.05f * (float) screenWidth, (float) screenHeight - (0.1f * (float) screenHeight), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        textRenderer.renderText("Score: " + std::to_string(playerScore), (float) screenWidth - (0.15f * (float) screenWidth), (float) screenHeight - (0.1f * (float) screenHeight), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

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

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, GLuint VAO, GLuint VBO)
{
    // activate corresponding render state
    s.use();
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + (float) ch.Bearing.x * scale;
        float ypos = y - (float) (ch.Size.y - ch.Bearing.y) * scale;

        float w = (float) ch.Size.x * scale;
        float h = (float) ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (float) (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
