#include "Shader.h"
#include <logger/logger.h>

Shader::Shader(GLenum shaderType, const char *shaderText) {
    this->id = glCreateShader(shaderType);
    LOGI("Creating shader %d", this->id);
    glShaderSource(this->id, 1, &shaderText, nullptr);
    glCompileShader(this->id);
    int status = -1;
    glGetShaderiv(this->id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &errorLength);
        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);
        glGetShaderInfoLog(this->id, errorLength, 0, errorMessage.data());
        LOGE("Failed to compile the shader:\n %s", errorMessage.data());
        LOGE("Faulty shader:\n %s", shaderText);
        exit(1);
    }
};

ShaderPtr Shader::fromFile(GLenum shaderType, const std::string& filename) {
    std::ifstream fin(filename);
    std::stringstream buffer;
    buffer << fin.rdbuf();
    return std::make_shared<Shader>(shaderType, buffer.str().data());
}

Shader::~Shader() {
    glDeleteShader(this->id);
}

ShaderProgram::ShaderProgram(ShaderPtr vertexShader, ShaderPtr fragShader) {
    this->program = glCreateProgram();
    LOGI("Created program %d", this->program);
    glAttachShader(this->program, vertexShader->id);
    glAttachShader(this->program, fragShader->id);
    glLinkProgram(this->program);
    int status = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLength);
        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);
        glGetProgramInfoLog(program, errorLength, 0, errorMessage.data());
        LOGE("Failed to link the program:\n %s", errorMessage.data());
        exit(1);
    }
}

void ShaderProgram::use() const {
    glUseProgram(this->program);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(this->program);
}

void ShaderProgram::setMat4Uniform(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setVec4Uniform(const std::string &name, const glm::vec4 &vec) const {
    glUniform4fv(getLocation(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setIntUniform(const std::string &name, const int &value) const {
    glUniform1i(getLocation(name), value);
}

void ShaderProgram::setFloatUniform(const std::string &name, const float &value) const {
    glUniform1f(getLocation(name), value);
}

GLint ShaderProgram::getLocation(const std::string &name) const {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    if (location == -1) {
        LOGE("Failed to find %s in shader program %d", name.c_str(), this->program);
    }
    return location;
}

void ShaderProgram::setVec3Uniform(const std::string &name, const glm::vec3 &vec) const {
    glUniform3fv(getLocation(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setMat3Uniform(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}
