#pragma once

#include "Object.h"

class Camera: public Object {
public:
    virtual void on_activated(GamePtr game) {}
    virtual void on_deactivated(GamePtr game) {}

    glm::mat4 proj_matrix() const {
        return _proj_matrix;
    }

    glm::mat4 view_matrix() const {
        return _view_matrix;
    }

    virtual void update_view_matrix();

protected:
    glm::mat4 _proj_matrix;
    glm::mat4 _view_matrix;
};

using CameraPtr = std::shared_ptr<Camera>;

class FreeMovingCamera: public Camera {
public:
    FreeMovingCamera(float aspect_ratio, float fov_degree, float angular_speed = 0.01, float spatial_speed = 3.0)
            : aspect_ratio(aspect_ratio), fov_degree(fov_degree), angular_speed(angular_speed), spatial_speed(spatial_speed) { };

    void init(GamePtr game) override;
    void update(GamePtr game, float dt) override;


    void update_view_matrix() override;

    void set_pitch(float pitch);
    void set_yaw(float yaw);
    float get_pitch();
    float get_yaw();

protected:
    float aspect_ratio, fov_degree;
    float angular_speed, spatial_speed;

    float pitch = glm::radians(-180.0f);
    float yaw = glm::radians(-90.0f);

    bool on_mouse_move(double x_pos, double y_pos, double x_offset, double y_offset);
    GamePtr game;
};

class Terrain;
using TerrainPtr = std::shared_ptr<Terrain>;

class FirstPersonCamera: public FreeMovingCamera {
public:
    FirstPersonCamera(TerrainPtr terrain, float fov_degree, float aspect_ratio, float height_above_surface, float smooth_factor = 0.9f,
                      float angular_speed = 0.01, float spatial_speed = 10.0)
            : FreeMovingCamera(aspect_ratio, fov_degree, angular_speed, spatial_speed), terrain(terrain),
                height_above_surface(height_above_surface), smooth_factor(smooth_factor) { };

    void update(GamePtr game, float dt) override;

protected:
    TerrainPtr terrain;
    float height_above_surface;
    float smooth_factor;
};
