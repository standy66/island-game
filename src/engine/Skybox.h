#pragma once

#include <memory>
#include "Object.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"

class Skybox : public Object {
public:
    Skybox(const std::string& right, const std::string& left,
           const std::string& top, const std::string& bottom,
           const std::string& back, const std::string& front);

    void render(GamePtr game, CameraPtr camera) override;

protected:
    TexturePtr cubemapTexture;
    MeshPtr mesh;
    ShaderProgramPtr program;
    SamplerPtr cubemapSampler;
};


using SkyboxPtr = std::shared_ptr<Skybox>;