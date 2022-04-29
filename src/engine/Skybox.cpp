#include "Skybox.h"
#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include "Camera.h"

Skybox::Skybox(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom,
               const std::string &back, const std::string &front) {
    this->cubemapTexture = CubeTexture::load_cube_texture(right, left, top, bottom, back, front, false);
    std::vector<float> points = {
            // front
            -.5f, -.5f, .5f,
            .5f, -.5f, .5f,
            .5f, .5f, .5f,
            -.5f, .5f, .5f,
            // back
            -.5f, -.5f, -.5f,
            .5f, -.5f, -.5f,
            .5f, .5f, -.5f,
            -.5f, .5f, -.5f,
    };

    std::vector<unsigned int> indices = {
            // front
            1, 0, 2,
            3, 2, 0,
            // top
            5, 1, 6,
            2, 6, 1,
            // back
            6, 7, 5,
            4, 5, 7,
            // bottom
            0, 4, 3,
            7, 3, 4,
            // left
            5, 4, 1,
            0, 1, 4,
            // right
            2, 3, 6,
            7, 6, 3,
    };
    this->mesh = std::make_shared<Mesh>(points, indices);
    this->mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    ShaderPtr vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/skybox.vert");
    ShaderPtr fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/skybox.frag");
    this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);

    this->cubemapSampler = std::make_shared<Sampler>(GL_LINEAR, GL_LINEAR,
                                                     GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Skybox::render(GamePtr game, CameraPtr camera) {
    this->program->use();
    auto projMat = camera->proj_matrix();
    auto viewMat = glm::mat4(glm::mat3(camera->view_matrix()));

    this->program->setMat4Uniform("viewMatrix", viewMat);
    this->program->setMat4Uniform("projMatrix", projMat);

    this->cubemapSampler->bind(0, this->cubemapTexture);
    this->program->setIntUniform("cubemapTexture", 0);

    glDepthMask(GL_FALSE);
    mesh->draw();
    glDepthMask(GL_TRUE);
}
