#pragma once

#include "camera.h"
#include "keys.h"

namespace core {

enum class ActorType {
    Default, FPS
};

template <ActorType type = ActorType::Default>
class CommonActor {
public:
    struct Config {
        Config() {}
        float mouse_sensitivity = 0.005f;
        float rotation_speed = 2.0f;
        float speed = 5.0f;
        float speedup_factor = 3.0f;
    };

    CommonActor(Camera&& camera, Config config = {})
        : camera(std::move(camera))
        , config(config)
    {}

    void keyAction(KeyAction action, Key key) {
        if (action != KeyAction::Press && action != KeyAction::Release)
            return;
        bool is_pressed = action == KeyAction::Press;
        switch (key) {
        case Key::W: move_forward       = is_pressed; break;
        case Key::S: move_backward      = is_pressed; break;
        case Key::A: move_left          = is_pressed; break;
        case Key::D: move_right         = is_pressed; break;
        case Key::E: rotate_left        = is_pressed; break;
        case Key::Q: rotate_right       = is_pressed; break;
        case Key::SPACE:      move_up   = is_pressed; break;
        case Key::LEFT_SHIFT: move_down = is_pressed; break;
        case Key::LEFT_CONTROL: speedup = is_pressed; break;
        default: break;
        }
    }

    void mouseMoveDelta(glm::vec2 delta) {
        switch (type) {
        case ActorType::FPS:    mouseMoveDeltaFPS(delta);    break;
        case ActorType::Default: mouseMoveDeltaDefault(delta); break;
        }
    }

    void precessMovement(float delta) {
        camera.move(
            {
                .forward  = move_forward,
                .backward = move_backward,
                .left     = move_left,
                .right    = move_right,
                .up       = move_up,
                .down     = move_down,
            },
            delta * config.speed * (speedup ? config.speedup_factor : 1.0f));
        if (type == ActorType::Default && rotate_right != rotate_left) {
            auto angle = delta * config.rotation_speed * (float(rotate_left) - float(rotate_right));
            camera.rotate(glm::angleAxis(angle, camera.front));
        }
    }

    glm::mat4 viewProj() const { return camera.viewProj(); }
    glm::vec3 pos() const { return camera.pos; }
    glm::vec3 dir() const { return camera.front; }
private:
    void mouseMoveDeltaDefault(glm::vec2 delta) {
        float angle = glm::length(delta) * config.mouse_sensitivity;
        delta = glm::normalize(delta);
        glm::vec3 dir{-camera.right * delta.x + camera.up * delta.y};
        glm::vec3 front{camera.front};
        camera.rotate(glm::angleAxis(angle, glm::cross(dir, front)));
    }

    void mouseMoveDeltaFPS(glm::vec2 delta) {
        camera.rotate(glm::angleAxis(-delta.y * config.mouse_sensitivity, camera.right));
        camera.rotate(glm::angleAxis( delta.x * config.mouse_sensitivity, glm::vec3{0, -1, 0}));
    }

    Camera camera;
    Config config;
    bool move_forward  = false;
    bool move_backward = false;
    bool move_left     = false;
    bool move_right    = false;
    bool move_up       = false;
    bool move_down     = false;
    bool rotate_left   = false;
    bool rotate_right  = false;
    bool speedup       = false;
};

using FPSActor = CommonActor<ActorType::FPS>;
using Actor = CommonActor<ActorType::Default>;

} // namespace core
