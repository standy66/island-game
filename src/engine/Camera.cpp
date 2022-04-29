
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Game.h"
#include "Terrain.h"

#include <algorithm>

void FreeMovingCamera::update(GamePtr game, float dt) {
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f) * this->rotation;
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * this->rotation;

    auto spatial_speed = this->spatial_speed;

    if (game->get_key(GLFW_KEY_LEFT_SHIFT)) {
        spatial_speed *= 3;
    }

    if (game->get_key(GLFW_KEY_W)) {
        this->position += spatial_speed * dt * forward;
    }
    if (game->get_key(GLFW_KEY_S)) {
        this->position -= spatial_speed * dt * forward;
    }
    if (game->get_key(GLFW_KEY_A)) {
        this->position -= spatial_speed * dt * right;
    }
    if (game->get_key(GLFW_KEY_D)) {
        this->position += spatial_speed * dt * right;
    }
    this->update_view_matrix();
}

void FreeMovingCamera::init(GamePtr game) {
    using namespace std::placeholders;
    game->add_mouse_move_callback(std::bind(&FreeMovingCamera::on_mouse_move, this, _1, _2, _3, _4));
    this->game = game;
    this->_proj_matrix = glm::perspective(glm::radians(fov_degree), aspect_ratio, 0.1f, 500.0f);
    this->rotation = glm::angleAxis(glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

bool FreeMovingCamera::on_mouse_move(double x_pos, double y_pos, double x_offset, double y_offset) {
    //if (this->game->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);
        pitch += angular_speed * y_offset;
        pitch = std::min(pitch, glm::radians(-90.0f));
        pitch = std::max(pitch, glm::radians(-270.0f));
        yaw += angular_speed * x_offset;
        rotation = (glm::angleAxis(pitch, right) *
                    glm::angleAxis(yaw, up));
        rotation = glm::normalize(rotation);
    //}
    return true;
}

void FreeMovingCamera::update_view_matrix() {

    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);

    this->rotation = (glm::angleAxis(pitch, right) *
                      glm::angleAxis(yaw, up));
    this->rotation = glm::normalize(this->rotation);
    Camera::update_view_matrix();
}

void FreeMovingCamera::set_pitch(float pitch) {
    this->pitch = pitch;
}

void FreeMovingCamera::set_yaw(float yaw) {
    this->yaw = yaw;
}

float FreeMovingCamera::get_pitch() {
    return this->pitch;
}

float FreeMovingCamera::get_yaw() {
    return this->yaw;
}

void FirstPersonCamera::update(GamePtr game, float dt) {
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f) * this->rotation;
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * this->rotation;

    auto spatial_speed = this->spatial_speed;

    if (game->get_key(GLFW_KEY_LEFT_SHIFT)) {
        spatial_speed *= 4;
    }

    float old_y = this->position.y;

    if (game->get_key(GLFW_KEY_W)) {
        this->position += spatial_speed * dt * forward;
    }
    if (game->get_key(GLFW_KEY_S)) {
        this->position -= spatial_speed * dt * forward;
    }
    if (game->get_key(GLFW_KEY_A)) {
        this->position -= spatial_speed * dt * right;
    }
    if (game->get_key(GLFW_KEY_D)) {
        this->position += spatial_speed * dt * right;
    }
    float surf_height = this->terrain->get_height(this->position.x, this->position.z);
    this->position.y = (smooth_factor * old_y + (1 - smooth_factor) * (surf_height + height_above_surface));
    this->update_view_matrix();
}

void Camera::update_view_matrix() {
    this->_view_matrix = glm::toMat4(-this->rotation) * glm::translate(glm::mat4(1.0f), -this->position);
}
