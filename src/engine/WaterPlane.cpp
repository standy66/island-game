//
// Created by Andrew Stepanov on 24/05/2017.
//

#include "WaterPlane.h"

#include "Camera.h"
#include "Game.h"
#include "BasicObjects.h"
#include <glm/gtx/string_cast.hpp>

void WaterPlane::prepare_render(GamePtr game, CameraPtr c) {
    auto camera = std::dynamic_pointer_cast<FreeMovingCamera>(c);
    glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);

    camera->disable();
    this->debug_quad_reflection->disable();
    this->debug_quad_refraction->disable();
    this->hide();


    this->refraction_fb->begin_render();
    this->terrain->set_clipping_plane(glm::vec4(normal, -this->position.y + 0.05));
    game->render(camera);
    this->refraction_fb->unbind();


    this->reflection_fb->begin_render();
    this->terrain->set_clipping_plane(glm::vec4(-normal, this->position.y + 0.05));
    glm::vec3 old_camera_pos = camera->get_position();
    glm::vec3 new_camera_pos = this->position + glm::reflect(old_camera_pos - this->position, normal);
    float old_pitch = camera->get_pitch();
    float new_pitch = -(2 * glm::pi<float>() - (-old_pitch));
    camera->set_position(new_camera_pos);
    camera->set_pitch(new_pitch);
    camera->update_view_matrix();
    game->render(camera);
    camera->set_position(old_camera_pos);
    camera->set_pitch(old_pitch);
    camera->update_view_matrix();
    this->reflection_fb->unbind();


    this->terrain->set_clipping_plane(glm::vec4(0.0, 0.0, 0.0, 1.0));
    camera->enable();
    this->debug_quad_reflection->enable();
    this->debug_quad_refraction->enable();
    this->show();
}

void WaterPlane::render(GamePtr game, CameraPtr camera) {
    this->program->use();
    this->program->setMat4Uniform("modelMatrix", this->model_matrix());
    this->program->setMat4Uniform("viewMatrix", camera->view_matrix());
    this->program->setMat4Uniform("projMatrix", camera->proj_matrix());
    this->sampler->bind(0, this->reflection_fb->get_render_texture());
    this->sampler->bind(1, this->refraction_fb->get_render_texture());
    this->sampler->bind(2, this->dudv_texture);
    this->sampler->bind(3, this->normal_texture);
    this->program->setIntUniform("reflection_texture", 0);
    this->program->setIntUniform("refraction_texture", 1);
    this->program->setIntUniform("dudv_texture", 2);
    this->program->setIntUniform("normal_texture", 3);
    this->program->setFloatUniform("offset", global_time * wave_speed);
    LOGI("offset: %f", global_time * wave_speed);
    this->program->setVec3Uniform("cameraPosition", camera->get_position());

    this->mesh->draw();
}

WaterPlane::WaterPlane(TerrainPtr terrain)
        : terrain(terrain)
{

    this->set_scale(glm::vec3(1000.0f, 1000.0f, 1000.0f));
    this->set_position(glm::vec3(0.0f, 25.0f, 0.0f));
    std::vector<float> points = {
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
            0, 0,
            0, 1,
            1, 1,
            1, 0,
    };


    std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0,
    };


    this->mesh = std::make_shared<Mesh>(points, indices);
    this->mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->mesh->setAttribute(1, 2, GL_FLOAT, GL_FALSE, 0, 48);

    this->sampler = std::make_shared<Sampler>(GL_LINEAR, GL_LINEAR);

    this->reflection_fb = std::make_shared<Framebuffer>(1024, 1024);
    this->refraction_fb = std::make_shared<Framebuffer>(1024, 1024);
    this->debug_quad_reflection = std::make_shared<ScreenQuad>(0.5, 0.5, 1.0, 1.0,
                                                               this->reflection_fb->get_render_texture());

    this->debug_quad_refraction = std::make_shared<ScreenQuad>(0.0, 0.5, 1.0, 1.5,
                                                               this->refraction_fb->get_render_texture());


    ShaderPtr vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/water.vert");
    ShaderPtr fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/water.frag");
    this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
    this->dudv_texture = Texture::load_texture("data/water_dudv_2.jpg", false);
    this->normal_texture = Texture::load_texture("data/water_normal.jpg", false);

}

void WaterPlane::init(GamePtr game) {
    game->add_object(this->debug_quad_reflection);
    game->add_object(this->debug_quad_refraction);
}

void WaterPlane::update(GamePtr game, float dt) {
    this->global_time += dt;
}
