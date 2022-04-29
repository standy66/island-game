#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Game;
using GamePtr = Game*;

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

class Object {
public:
    Object();

    virtual void init(GamePtr game) {};
    virtual void update(GamePtr game, float dt) {};
    virtual void prepare_render(GamePtr game, CameraPtr camera) {};
    virtual void render(GamePtr game, CameraPtr camera) {};

    void set_position(const glm::vec3 &position);
    void set_rotation(const glm::quat &rotation);
    void set_scale(const glm::vec3 &scale);

    glm::vec3 get_position() const;
    glm::vec3 get_scale() const;
    glm::quat get_rotation() const;
    glm::mat4 model_matrix() const;

    void enable() {
        this->enabled = true;
    }

    void disable() {
        this->enabled = false;
    }

    void show() {
        this->visible = true;
    }

    void hide() {
        this->visible = false;
    }

    bool is_visible() {
        return this->visible && this->enabled;
    }

    bool is_enabled() {
        return this->enabled;
    }

protected:
    bool enabled;
    bool visible;

    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

using ObjectPtr = std::shared_ptr<Object>;

