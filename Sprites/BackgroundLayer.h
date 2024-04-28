
#ifndef LEARNOPENGL_BACKGROUNDLAYER_H
#define LEARNOPENGL_BACKGROUNDLAYER_H


#include "Sprite.h"
#include <string>

class BackgroundLayer : public Sprite {
public:
    BackgroundLayer() = default;
    ~BackgroundLayer();
    void initialize(GLuint texId_, int screenWidth_, int screenHeight_, float step_, float movementInterval_);
private:
    void update() override;
    void setVAO() override;

    float step;
};


#endif //LEARNOPENGL_BACKGROUNDLAYER_H
