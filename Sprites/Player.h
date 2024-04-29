
#ifndef LEARNOPENGL_PLAYER_H
#define LEARNOPENGL_PLAYER_H

#include "Sprite.h"
#include <unordered_map>

class Player : public Sprite {
public:
    void initialize(GLuint slowTexId_, GLuint forwardTexId, GLuint backwardTexId, GLuint hurtTexId, glm::vec3 pos_, glm::vec3 scale_,
                            float angle_, int screenWidth, int screenHeight,
                            int rows, int columns, int startingRow,
                            double animationInterval, double movementInterval);
    void move(float distance, std::unordered_map<int,bool>& wasdMap);
    void setSlow();
    void setForward();
    void setBackward();
    void setHurt();
    int getLives() const;
private:
    void update() override;
    void moveRight(float distance);
    void moveLeft(float distance);
    void moveUp(float distance);
    void moveDown(float distance);
    GLuint slowTexId;
    GLuint forwardTexId;
    GLuint backwardTexId;
    GLuint hurtTexId;
    double hurtInterval;
    double lastHurtTime;
    double cannotGetHurtInterval;
    bool isHurt;
    int lives;
};


#endif //LEARNOPENGL_PLAYER_H
