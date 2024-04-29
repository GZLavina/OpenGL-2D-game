
#ifndef LEARNOPENGL_TRASH_H
#define LEARNOPENGL_TRASH_H


#include "Sprite.h"
#include <vector>

class Trash : public Sprite {
public:
    Trash() = default;
    ~Trash();
    void initialize(GLuint texId_, float angle_, int screenWidth_, int screenHeight_, double movementInterval_, std::vector<GLuint> *texIds_);
private:
    void update() override;
    void reset();
    void rotate();
    int currentRotationDirection; // 0=left, 1=none, 2=right
    std::vector<GLuint> *texIds;
};


#endif //LEARNOPENGL_TRASH_H
