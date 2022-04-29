#pragma once

#include "perlin.h"
#include "Shader.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "Texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>
#include <logger/logger.h>
#include <glm/gtx/string_cast.hpp>

class Terrain;

using TerrainPtr = std::shared_ptr<Terrain>;

class Terrain: public Object {
public:
    Terrain(int points_x, int points_z, int tiles_x, int tiles_z, float size_x, float size_y, float size_z, int octaves)
        :perlin(), tiles_x(tiles_x), tiles_z(tiles_z), size_x(size_x), size_y(size_y), size_z(size_z), octaves(octaves)
    {
        std::vector<std::vector<glm::vec3>> points(points_x, std::vector<glm::vec3>(points_z, glm::vec3(0.0f)));
        std::vector<float> point_buf;
        std::vector<unsigned int> ind_buf;

        for (int i = 0; i < points_x; i++) {
            for (int j = 0; j < points_z; j++) {
                float x = (float) i / points_x;
                float z = (float) j / points_z;
                float y = (perlin.octaveNoise(x * tiles_x, z * tiles_z, octaves) * 0.5 + 0.5);
                points[i][j] = glm::vec3(x * size_x, y * size_y, z * size_z);
            }
        }

        for (int i = 0; i < points_x; i++) {
            for (int j = 0; j < points_z; j++) {
                point_buf.push_back(points[i][j].x);
                point_buf.push_back(points[i][j].y);
                point_buf.push_back(points[i][j].z);

                std::vector<glm::vec3> normals;
                if ((i > 0) && (j > 0)) {
                    normals.push_back(glm::cross(points[i][j - 1] - points[i][j],
                                                              points[i - 1][j] - points[i][j]));
                }
                if ((i < points_x - 1) && (j < points_z - 1)) {
                    normals.push_back(glm::cross(points[i][j + 1] - points[i][j],
                                                              points[i + 1][j] - points[i][j]));
                }
                if ((i > 0) && (j < points_z - 1)) {
                    normals.push_back(glm::cross(points[i - 1][j] - points[i][j],
                                                              points[i - 1][j + 1] - points[i][j]));
                    normals.push_back(glm::cross(points[i - 1][j + 1] - points[i][j],
                                                              points[i][j + 1] - points[i][j]));
                }
                if ((i < points_x - 1) && (j > 0)) {
                    normals.push_back(glm::cross(points[i + 1][j] - points[i][j],
                                                              points[i + 1][j - 1] - points[i][j]));
                    normals.push_back(glm::cross(points[i + 1][j - 1] - points[i][j],
                                                              points[i][j - 1] - points[i][j]));
                }
                glm::vec3 normal = mix_normals(normals);
                point_buf.push_back(normal.x);
                point_buf.push_back(normal.y);
                point_buf.push_back(normal.z);


                std::vector<glm::vec3> tangents;
                if (i > 0) {
                    tangents.push_back(points[i][j] - points[i - 1][j]);
                }
                if (i < points_x - 1) {
                    tangents.push_back(points[i + 1][j] - points[i][j]);
                }
                glm::vec3 tangent = mix_normals(tangents);
                point_buf.push_back(tangent.x);
                point_buf.push_back(tangent.y);
                point_buf.push_back(tangent.z);

                std::vector<glm::vec3> bitangents;
                if (j > 0) {
                    bitangents.push_back(points[i][j] - points[i][j - 1]);
                }
                if (j < points_z - 1) {
                    bitangents.push_back(points[i][j + 1] - points[i][j]);
                }
                glm::vec3 bitangent = mix_normals(bitangents);
                point_buf.push_back(bitangent.x);
                point_buf.push_back(bitangent.y);
                point_buf.push_back(bitangent.z);

                // UV coords
                float u = 10 * tiles_x * (float) i / points_x;
                float v = 10 * tiles_z * (float) j / points_z;
                point_buf.push_back(u);
                point_buf.push_back(v);

                // world 2D coords for mask
                point_buf.push_back((float) i / points_x);
                point_buf.push_back((float) j / points_z);
            }
        }

        for (int i = 0; i < points_x - 1; i++) {
            for (int j = 0; j < points_z - 1; j++) {
                unsigned int cur = i * points_z + j;
                unsigned int bottom = i * points_z + (j+1);
                unsigned int right = (i + 1) * points_z + j;
                unsigned int right_bottom = (i + 1) * points_z + (j + 1);
                ind_buf.push_back(cur); ind_buf.push_back(right); ind_buf.push_back(bottom);
                ind_buf.push_back(right); ind_buf.push_back(right_bottom); ind_buf.push_back(bottom);
            }
        }

        LOGI("Num triangles %d", ind_buf.size() / 3);
        LOGI("Num vertices %d", points.size() / 7);

        mesh = std::make_shared<Mesh>(point_buf, ind_buf);
        // position
        mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 0);
        // normal
        mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 3 * sizeof(float));
        // tangent
        mesh->setAttribute(2, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 6 * sizeof(float));
        // bitangent
        mesh->setAttribute(3, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 9 * sizeof(float));
        // UV
        mesh->setAttribute(4, 2, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 12 * sizeof(float));
        // terrain UV
        mesh->setAttribute(5, 2, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 14 * sizeof(float));

        ShaderPtr vertexShader = Shader::fromFile(GL_VERTEX_SHADER, "shader/terrain.vert");
        ShaderPtr fragmentShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/terrain.frag");
        this->program = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
        ShaderPtr wireframeFragShader = Shader::fromFile(GL_FRAGMENT_SHADER, "shader/terrain_wireframe.frag");
        this->wireframe_program = std::make_shared<ShaderProgram>(vertexShader, wireframeFragShader);

        this->texture[0] = Texture::load_texture("data/mask.png", true);
        this->texture[1] = Texture::load_texture("data/grass_1.jpg", false);
        this->texture[2] = Texture::load_texture("data/gravel.jpg", false);
        this->texture[3] = Texture::load_texture("data/sand.jpg", false);
        this->texture[4] = Texture::load_texture("data/snow1.png", true);

        this->texture[5] = Texture::load_texture("data/grass_1_norm.jpg", false);
        this->texture[6] = Texture::load_texture("data/gravel_norm.jpg", false);
        this->texture[7] = Texture::load_texture("data/sand_norm.jpg", false);
        this->texture[8] = Texture::load_texture("data/snow1_norm.jpg", false);

        for (int i = 0; i < 9; ++i) {
            sampler[i] = std::make_shared<Sampler>(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
        }
    }

    virtual void update(GamePtr game, float dt) {
        //this->rotation *= glm::angleAxis(10 * dt, glm::normalize(glm::vec3(2.0f, 2.0f, 0.0f)));
    };

    virtual void render(GamePtr game, CameraPtr camera) {
        program->use();
        auto modelMat = this->model_matrix();
        auto projMat = camera->proj_matrix();
        auto viewMat = camera->view_matrix();
        program->setMat4Uniform("modelMatrix", modelMat);
        program->setMat4Uniform("viewMatrix", viewMat);
        program->setMat4Uniform("projMatrix", projMat);

        for (int i = 0; i < 9; ++i) {
            this->sampler[i]->bind(i, this->texture[i]);
            this->program->setIntUniform(this->sampler_names[i], i);
        }
        program->setFloatUniform("xSpec", 0.0);
        program->setFloatUniform("ySpec", 0.03);
        program->setFloatUniform("zSpec", 0.05);
        program->setFloatUniform("wSpec", 0.2);
        program->setVec4Uniform("clipping_plane", this->clipping_plane);



        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mesh->draw();

        if (this->wireframe)
        {
            wireframe_program->use();
            wireframe_program->setMat4Uniform("modelMatrix", projMat * viewMat * modelMat);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            mesh->draw();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    };

    virtual float get_height(float x, float z) {
        return (perlin.octaveNoise(tiles_x * x / size_x, tiles_z * z / size_z, octaves) * 0.5 + 0.5) * size_y;
    }

    virtual void set_clipping_plane(glm::vec4 clipping_plane) {
        this->clipping_plane = clipping_plane;
    }

protected:
    ShaderProgramPtr program;
    ShaderProgramPtr wireframe_program;
    MeshPtr mesh;
    TexturePtr texture[9];
    SamplerPtr sampler[9];
    const static std::string sampler_names[];
    glm::vec4 clipping_plane = glm::vec4(0.0, 0.0, 0.0, 1.0);
    bool wireframe = false;
    PerlinNoise perlin;
    int tiles_x;
    int tiles_z;
    float size_x;
    float size_y;
    float size_z;
    int octaves;

private:
    glm::vec3 mix_normals(const std::vector<glm::vec3> &normals) {
        glm::vec3 normal(0.0f);
        for (const glm::vec3 &adj_normal: normals) {
            normal += glm::normalize(adj_normal);
        }
        normal /= normals.size();
        return normal;
    }

};

using TerrainPtr = std::shared_ptr<Terrain>;