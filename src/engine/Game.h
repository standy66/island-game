#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "Camera.h"
#include "Object.h"
#include "Skybox.h"
#include "Framebuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

typedef std::function<bool(int, int, int, int)> KeyCallback;
typedef std::function<bool(double, double, int, int, int)> MouseButtonCallback;
typedef std::function<bool(double, double, double, double)> MouseMoveCallback;
typedef std::function<bool(double, double)> ScrollCallback;

class Game {
public:
    Game(int width, int height): width_(width), height_(height) { }

    void set_active_camera(CameraPtr camera);
    void set_skybox(SkyboxPtr skybox);
    void add_object(ObjectPtr object);
    int init();
    void run();
    void update(float dt);
    void prepare_render(CameraPtr camera);
    void render(CameraPtr camera);

    void add_key_callback(KeyCallback callback);
    void add_mouse_button_callback(MouseButtonCallback callback);
    void add_mouse_move_callback(MouseMoveCallback callback);
    void add_scroll_callback(ScrollCallback callback);

    bool get_mouse_button(int button) const;
    bool get_key(int key) const;

public:
    FramebufferPtr screenBuffer;
    ObjectPtr screen_quad = nullptr;
    SkyboxPtr skybox = nullptr;
    CameraPtr active_camera;
    std::vector<ObjectPtr> objects;
    GLFWwindow* window;
    int width_, height_;
    double mouse_x_pos = 0.0;
    double mouse_y_pos = 0.0;
    std::vector<KeyCallback> key_callbacks;
    std::vector<MouseButtonCallback> mouse_button_callbacks;
    std::vector<MouseMoveCallback> mouse_move_callbacks;
    std::vector<ScrollCallback> scroll_callbacks;


    virtual void make_scene() = 0;


    void glfw_key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods);
    void glfw_window_size_callback(GLFWwindow *window, int width, int height);
    void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    void glfw_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos);
    void glfw_scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

private:
    void bind_callbacks();
};


using GamePtr = Game*;
