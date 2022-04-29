#pragma once

#include "Object.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Terrain.h"

class WaterPlane: public Object {
public:
    WaterPlane(TerrainPtr terrain);

    void init(GamePtr game) override;

    void update(GamePtr game, float dt) override;

    void prepare_render(GamePtr game, CameraPtr camera) override;

    void render(GamePtr game, CameraPtr camera) override;

private:
    float global_time = 0.0f;
    const float wave_speed = 0.02f;
    TexturePtr dudv_texture, normal_texture;
    TerrainPtr terrain;
    ObjectPtr debug_quad_reflection, debug_quad_refraction;
    FramebufferPtr reflection_fb, refraction_fb;
    SamplerPtr sampler;
    MeshPtr mesh;
    ShaderProgramPtr program;
};