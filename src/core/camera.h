#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace core {

class Camera {
public:
    enum class Direction {
        Forward, Backward, Left, Right, Up, Down
    };

    struct Directions {
        bool forward  : 1 = false;
        bool backward : 1 = false;
        bool left     : 1 = false;
        bool right    : 1 = false;
        bool up     : 1 = false;
        bool down    : 1 = false;
    };

    Camera(float screenWHRatio, glm::vec3 pos = {0, 0, 1}, glm::vec3 target = {0, 0, 0}, glm::vec3 up = {0, 1, 0});

    glm::mat4 viewProj() const;

    void rotate(glm::quat quaternion);
    void move(Directions dirs, float delta);
    void move(Direction direction, float delta);
// private:
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    float screenWHRatio;
};

} // namespace core
