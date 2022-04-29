#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Object.h"

glm::mat4 Object::model_matrix() const {
    //glm::translate(glm::scale(glm::mat4(1.0f), this->scale), this->position)
    return glm::mat4(scale.x, 0, 0, 0,
                     0, scale.y, 0, 0,
                     0, 0, scale.z, 0,
                     position.x, position.y, position.z, 1) * glm::toMat4(this->rotation);
}

void Object::set_position(const glm::vec3 &position) {
    this->position = position;
}

void Object::set_rotation(const glm::quat &rotation) {
    this->rotation = rotation;
}

void Object::set_scale(const glm::vec3 &scale) {
    this->scale = scale;
}

Object::Object()
    : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(), enabled(true), visible(true) { }

glm::vec3 Object::get_position() const {
    return this->position;
}

glm::vec3 Object::get_scale() const {
    return this->scale;
}

glm::quat Object::get_rotation() const {
    return this->rotation;
}
