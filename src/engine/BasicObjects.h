#pragma once

#include "Object.h"
#include "Shader.h"
#include "Mesh.h"



class Triangle: public Object {
public:
    Triangle() {
        std::cout << "Start constructor " << glGetError() << std::endl;
        std::vector<float> points =
                {
                        -0.5f, 0.5f, 0.0f,
                        0.5f, 0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f,
                        -0.5f, -0.5f, 0.0f,
                        1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 1.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 0.0f, 1.0f,
                };

        std::vector<unsigned int> indices =
                {
                        0, 3, 1,
                        3, 2, 1,
                };

        mesh = std::make_shared<Mesh>(points, indices);
        mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        mesh->setAttribute(1, 4, GL_FLOAT, GL_FALSE, 0, 48);

        auto vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/shader.vert");
        auto fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/shader.frag");
        this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
    }

    virtual void update(GamePtr game, float dt) {
        this->rotation *= glm::angleAxis(dt, glm::vec3(1.0f, 0.0f, 0.0f));
    };

    virtual void render(GamePtr game, CameraPtr camera) {
        program->use();
        auto modelMat = this->model_matrix();
        auto projMat = camera->proj_matrix();
        auto viewMat = camera->view_matrix();
        program->setMat4Uniform("modelMatrix", projMat * viewMat * modelMat);
        mesh->draw();
    };
protected:
    ShaderProgramPtr program;
    MeshPtr mesh;
};


class Cube: public Object {
public:
    Cube() {
        std::vector<float> points = {
                // front
                -.5f, -.5f,  .5f,
                .5f, -.5f,  .5f,
                .5f,  .5f,  .5f,
                -.5f,  .5f,  .5f,
                // back
                -.5f, -.5f, -.5f,
                .5f, -.5f, -.5f,
                .5f,  .5f, -.5f,
                -.5f,  .5f, -.5f,
                0.831f, 0.502f, 0.416f, 1.0f,
                0.502f, 0.169f, 0.082f, 1.0f,
                0.831f, 0.765f, 0.416f, 1.0f,
                0.502f, 0.431f, 0.082f, 1.0f,
                0.384f, 0.318f, 0.573f, 1.0f,
                0.157f, 0.09f, 0.345f,  1.0f,
                0.298f, 0.596f, 0.427f, 1.0f,
                0.059f, 0.357f, 0.188f, 1.0f,
        };

        std::vector<unsigned int> indices = {
                // front
                0, 1, 2,
                2, 3, 0,
                // top
                1, 5, 6,
                6, 2, 1,
                // back
                7, 6, 5,
                5, 4, 7,
                // bottom
                4, 0, 3,
                3, 7, 4,
                // left
                4, 5, 1,
                1, 0, 4,
                // right
                3, 2, 6,
                6, 7, 3,
        };

        mesh = std::make_shared<Mesh>(points, indices);

        mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        mesh->setAttribute(1, 4, GL_FLOAT, GL_FALSE, 0, 96);

        ShaderPtr vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/shader.vert");
        ShaderPtr fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/shader.frag");
        this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
        ShaderPtr wireframeFragShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/wireframe.frag");
        this->wireframe_program = std::make_shared<ShaderProgram>(vertexShader, wireframeFragShader);
    }

    virtual void update(GamePtr game, float dt) {
        this->rotation *= glm::angleAxis(10 * dt, glm::normalize(glm::vec3(2.0f, 2.0f, 0.0f)));
    };

    virtual void render(GamePtr game, CameraPtr camera) {
        program->use();
        auto modelMat = this->model_matrix();
        auto projMat = camera->proj_matrix();
        auto viewMat = camera->view_matrix();
        program->setMat4Uniform("modelMatrix", projMat * viewMat * modelMat);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mesh->draw();

        wireframe_program->use();
        wireframe_program->setMat4Uniform("modelMatrix", projMat * viewMat * modelMat);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mesh->draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };
protected:
    ShaderProgramPtr program;
    ShaderProgramPtr wireframe_program;
    MeshPtr mesh;
};


class ScreenQuad : public Object {
public:
    ScreenQuad(float left, float bottom, float right, float top, TexturePtr texture)
        :texture(texture)
    {
        std::vector<float> points = {
                left, bottom, 0.0,
                left, top, 0.0,
                right, top, 0.0,
                right, bottom, 0.0,
                0, 0,
                0, 1,
                1, 1,
                1, 0,
        };

        std::vector<unsigned int> indices = {
                1, 0, 2,
                3, 2, 0,
        };


        this->mesh = std::make_shared<Mesh>(points, indices);
        this->mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        this->mesh->setAttribute(1, 2, GL_FLOAT, GL_FALSE, 0, 48);

        ShaderPtr vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/quad.vert");
        ShaderPtr fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/quad.frag");
        this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);

        this->sampler = std::make_shared<Sampler>();
    }

    void render(GamePtr game, CameraPtr camera) override {
        this->program->use();

        this->sampler->bind(0, this->texture);
        this->program->setIntUniform("renderTexture", 0);

        mesh->draw();
    }

protected:
    MeshPtr mesh;
    ShaderProgramPtr program;
    TexturePtr texture;
    SamplerPtr sampler;
};