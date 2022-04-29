#pragma once

#include <string>
#include <cstdio>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL 100500
#endif

#define EMPTY

#if LOG_LEVEL >= 3
#define LOGD(fmt, ...) { \
    double time = glfwGetTime(); \
    long millis = int(time * 1000); \
    fprintf(stderr, (std::string("[DEBUG ") + __FILE__ + ":" + std::to_string(__LINE__) + " " + std::to_string(millis) + "ms] " + fmt + "\n").c_str(), __VA_ARGS__); \
}
#else
#define LOGD(fmt, ...)
#endif

#if LOG_LEVEL >= 2
#define LOGI(fmt, ...) { \
    double time = glfwGetTime(); \
    long millis = int(time * 1000); \
    fprintf(stderr, (std::string("[INFO ") + __FILE__ + ":" + std::to_string(__LINE__) + " " + std::to_string(millis) + "ms] " + fmt + "\n").c_str(), __VA_ARGS__); \
}
#else
#define LOGI(fmt, ...)
#endif

#if LOG_LEVEL >= 1
#define LOGW(fmt, ...) { \
    double time = glfwGetTime(); \
    long millis = int(time * 1000); \
    fprintf(stderr, (std::string("[WARN ") + __FILE__ + ":" + std::to_string(__LINE__) + " " + std::to_string(millis) + "ms] " + fmt + "\n").c_str(), __VA_ARGS__); \
}

#else
#define LOGW(fmt, ...)
#endif


#if LOG_LEVEL >= 0
#define LOGE(fmt, ...) { \
    double time = glfwGetTime(); \
    long millis = int(time * 1000); \
    fprintf(stderr, (std::string("[ERROR ") + __FILE__ + ":" + std::to_string(__LINE__) + " " + std::to_string(millis) + "ms] " + fmt + "\n").c_str(), __VA_ARGS__); \
}
#else
#define LOGE(fmt, ...)
#endif

