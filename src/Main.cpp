#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtx/quaternion.hpp>
#include <engine/WaterPlane.h>

#include "engine/Game.h"

#include "logger/logger.h"

#include "engine/BasicObjects.h"
#include "engine/Terrain.h"

class IslandGame: public Game {
public:
  IslandGame(): Game(1280, 720) { }
protected:
    virtual void make_scene() override {

        auto terrain = std::make_shared<Terrain>(600, 600, 6, 6, 600.0, 50.0, 600.0, 9);
        this->add_object(terrain);

        float aspect_ratio = static_cast<float>(width_) / height_;
        float fov = 80;
        auto camera = std::make_shared<FirstPersonCamera>(terrain, fov, aspect_ratio, -5.0f);
        this->add_object(camera);
        this->set_active_camera(camera);

        auto skybox = std::make_shared<Skybox>("data/right.png", "data/left.png",
                                               "data/top.png", "data/bottom.png",
                                               "data/back.png", "data/front.png");
        this->set_skybox(skybox);

        auto plane = std::make_shared<WaterPlane>(terrain);
        this->add_object(plane);

    }
};


int main(int argc, char** argv)
{
    IslandGame game;
    game.init();
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    game.run();
    return 0;
}
