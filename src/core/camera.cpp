#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/quaternion.hpp"

namespace core {

Camera::Camera(float screenWHRatio, glm::vec3 pos, glm::vec3 target, glm::vec3 worldUp)
    : pos(pos)
    , front(glm::normalize(target - pos))
    , right(glm::cross(front, glm::normalize(worldUp)))
    , up(glm::cross(right, front))
    , screenWHRatio(screenWHRatio)
{}

glm::mat4 Camera::viewProj() const {
    return glm::perspective(45.0f, screenWHRatio, 0.1f, 100.0f) * 
           glm::lookAt(pos, pos + front, up);
}

void Camera::move(Directions dirs, float delta) {
    glm::vec3 direction =
        front * (float(dirs.forward) - float(dirs.backward)) +
        up    * (float(dirs.up)      - float(dirs.down)) +
        right * (float(dirs.right)   - float(dirs.left));
    if (direction != glm::zero<glm::vec3>())
        pos += glm::normalize(direction) * delta;
}


void Camera::move(Direction direction, float delta) {
    switch (direction) {
    case Direction::Forward:  pos += front * delta; break;
    case Direction::Backward: pos -= front * delta; break;
    case Direction::Left:     pos -= right * delta; break;
    case Direction::Right:    pos += right * delta; break;
    case Direction::Down:     pos -= up    * delta; break;
    case Direction::Up:       pos += up    * delta; break;

    }
}

void Camera::rotate(glm::quat quaternion) {
    front = glm::rotate(quaternion, front);
    right = glm::rotate(quaternion, right);
    up    = glm::rotate(quaternion, up);
}

} // namespace core
