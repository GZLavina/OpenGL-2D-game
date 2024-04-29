
#ifndef LEARNOPENGL_FISH_H
#define LEARNOPENGL_FISH_H


#include "Sprite.h"
#include "glm/vec3.hpp"
#include <unordered_map>

class Fish : public Sprite {
public:
    Fish() = default;
    ~Fish();
    void initialize(GLuint texId_, glm::vec3 scale_, float angle_, float step_, int screenWidth_, int screenHeight_,
                    int rows, int columns, int startingRow,
                    double animationInterval_, double movementInterval_, std::unordered_map<GLuint,glm::vec3> *texIdToScaleMap_);
    void reset();
private:
    void update() override;
    float step;
    std::unordered_map<GLuint,glm::vec3> *texIdToScaleMap;
};


#endif //LEARNOPENGL_FISH_H
