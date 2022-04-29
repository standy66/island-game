#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

class Shader {
    friend class ShaderProgram;
public:
    Shader() = delete;
    Shader(const Shader& other) = delete;

    Shader(GLenum shaderType, const char* shaderText);
    static ShaderPtr fromFile(GLenum shaderType, const std::string& filename);
    ~Shader();

private:
    GLuint id;
};


class ShaderProgram;
using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

class ShaderProgram {
public:
    ShaderProgram() = delete;
    ShaderProgram(const ShaderProgram& other) = delete;

    ShaderProgram(ShaderPtr vertexShader, ShaderPtr fragShader);

    void setMat4Uniform(const std::string& name, const glm::mat4& mat) const;
    void setMat3Uniform(const std::string& name, const glm::mat3& mat) const;
    void setVec4Uniform(const std::string& name, const glm::vec4& vec) const;
    void setVec3Uniform(const std::string& name, const glm::vec3& vec) const;
    void setIntUniform(const std::string& name, const int& value) const;
    void setFloatUniform(const std::string& name, const float& value) const;

    ~ShaderProgram();

    void use() const;

private:
    GLint getLocation(const std::string& name) const;
    GLuint program;
};