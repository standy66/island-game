#include "Game.h"
#include "BasicObjects.h"
#include <logger/logger.h>
#include <functional>


void Game::run() {
    glfwSwapInterval(1);
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double dt = currentTime - lastTime;
        nbFrames++;
        lastTime = currentTime;
        if (nbFrames % 60 == 0){
            printf("%f ms\n", dt * 1000);
        }
        glfwPollEvents();
        this->update(dt);

        this->prepare_render(this->active_camera);

        this->screenBuffer->begin_render();
        this->render(this->active_camera);
        this->screenBuffer->unbind();

        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->screen_quad->update(this, dt);
        this->screen_quad->render(this, nullptr);

        glfwSwapBuffers(window);

    }
}


void Game::update(float dt) {
    if (this->skybox != nullptr) {
        if (this->skybox->is_enabled()) {
            this->skybox->update(this, dt);
        }
    }

    for (ObjectPtr obj: this->objects) {
        if (obj->is_enabled()) {
            obj->update(this, dt);
        }
    }
}


void Game::prepare_render(CameraPtr camera) {
    if (this->skybox != nullptr) {
        if (this->skybox->is_visible()) {
            this->skybox->prepare_render(this, camera);
        }
    }

    for (ObjectPtr obj: this->objects) {
        if (obj->is_visible()) {
            obj->prepare_render(this, camera);
        }
    }
}


void Game::render(CameraPtr camera) {
    if (this->skybox != nullptr) {
        if (this->skybox->is_visible()) {
            this->skybox->render(this, camera);
        }
    }

    for (ObjectPtr obj: this->objects) {
        if (obj->is_visible()) {
            obj->render(this, camera);
        }
    }
}



int Game::init() {
    if (!glfwInit())
    {
        LOGE("Failed to initilize GLFW", nullptr);
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

    // Open a window and create its OpenGL context
    this->window = glfwCreateWindow( width_, height_, "Game", NULL, NULL);
    if (this->window == NULL){
        LOGE("Failed to open OpenGL window", nullptr);
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        LOGE("Failed to initialize GLEW", nullptr);
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_CLIP_DISTANCE0);

    this->bind_callbacks();

    std::cout << glGetError() << std::endl << std::flush;
    this->make_scene();

    this->screenBuffer = std::make_shared<Framebuffer>(2048, 2048);
    this->screen_quad = std::make_shared<ScreenQuad>(-1.0, -1.0, 1.0, 1.0, this->screenBuffer->get_render_texture());
    this->screen_quad->init(this);

    return 0;
}


void Game::bind_callbacks() {
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(this->window, [](GLFWwindow* window, int key, int scan_code, int action, int mods) {
        auto pointer = static_cast<Game*>(glfwGetWindowUserPointer(window));
        pointer->glfw_key_callback(window, key, scan_code, action, mods);
    });

    glfwSetWindowSizeCallback(this->window, [](GLFWwindow *window, int width, int height) {
        auto pointer = static_cast<Game*>(glfwGetWindowUserPointer(window));
        pointer->glfw_window_size_callback(window, width, height);
    });

    glfwSetMouseButtonCallback(this->window, [](GLFWwindow *window, int button, int action, int mods) {
        auto pointer = static_cast<Game*>(glfwGetWindowUserPointer(window));
        pointer->glfw_mouse_button_callback(window, button, action, mods);
    });

    glfwSetCursorPosCallback(this->window, [](GLFWwindow *window, double x_pos, double y_pos) {
        auto pointer = static_cast<Game*>(glfwGetWindowUserPointer(window));
        pointer->glfw_cursor_pos_callback(window, x_pos, y_pos);
    });

    glfwSetScrollCallback(this->window, [](GLFWwindow *window, double x_offset, double y_offset) {
        auto pointer = static_cast<Game*>(glfwGetWindowUserPointer(window));
        pointer->glfw_scroll_callback(window, x_offset, y_offset);
    });
}

void Game::set_active_camera(CameraPtr camera) {
    if (this->active_camera != nullptr) {
        this->active_camera->on_deactivated(this);
    }
    this->active_camera = camera;
    camera->on_activated(this);
}

void Game::add_object(ObjectPtr object) {
    this->objects.push_back(object);
    object->init(this);
}

void Game::glfw_key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods) {
    LOGD("Key callback %d", key);

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(this->window, GL_TRUE);
            return;
        }
    }


    for (KeyCallback& callback: this->key_callbacks) {
        if (callback(key, scan_code, action, mods)) {
            break;
        }
    }
}

void Game::glfw_window_size_callback(GLFWwindow *window, int width, int height) {
    LOGD("Window size callback %d %d", width, height);
}

void Game::glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    LOGD("Mouse button callback %d", button);

    for (MouseButtonCallback& callback: this->mouse_button_callbacks) {
        if (callback(this->mouse_x_pos, this->mouse_y_pos, button, action, mods)) {
            break;
        }
    }
}

void Game::glfw_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos) {
    LOGD("Cursor callback %f %f", x_pos, y_pos);
    for (MouseMoveCallback& callback: this->mouse_move_callbacks) {
        if (callback(x_pos, y_pos, x_pos - this->mouse_x_pos, y_pos - this->mouse_y_pos)) {
            break;
        }
    }
    this->mouse_x_pos = x_pos;
    this->mouse_y_pos = y_pos;
}

void Game::glfw_scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    LOGD("Scroll callback %f %f", x_offset, y_offset);
    for (ScrollCallback& callback: this->scroll_callbacks) {
        if (callback(x_offset, y_offset)) {
            break;
        }
    }
}

void Game::add_key_callback(KeyCallback callback) {
    this->key_callbacks.push_back(callback);
}

void Game::add_mouse_button_callback(MouseButtonCallback callback) {
    this->mouse_button_callbacks.push_back(callback);
}

void Game::add_mouse_move_callback(MouseMoveCallback callback) {
    this->mouse_move_callbacks.push_back(callback);
}

void Game::add_scroll_callback(ScrollCallback callback) {
    this->scroll_callbacks.push_back(callback);
}

bool Game::get_mouse_button(int button) const {
    return glfwGetMouseButton(this->window, button) == GLFW_PRESS;
}

bool Game::get_key(int key) const {
    return glfwGetKey(this->window, key) == GLFW_PRESS;
}

void Game::set_skybox(SkyboxPtr skybox) {
    this->skybox = skybox;
    this->skybox->init(this);
}

