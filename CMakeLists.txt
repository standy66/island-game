cmake_minimum_required(VERSION 3.10)

project(island-game VERSION 0.1)

option(LOG_LEVEL "Logging level" 3)

file(GLOB SRC_FILES
        src/engine/*.cpp
        src/engine/*.h
        src/logger/logger.h
        src/Main.cpp)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glew)
add_subdirectory(vendor/glm)

file(GLOB SOIL2_SRC_FILES
        vendor/SOIL2/src/SOIL2/*.c)
add_library(soil2 ${SOIL2_SRC_FILES})
target_include_directories(soil2 PUBLIC vendor/SOIL2/src/SOIL2)


add_executable(island-game ${SRC_FILES})
target_compile_definitions(island-game PRIVATE LOG_LEVEL=${LOG_LEVEL})
target_include_directories(island-game PRIVATE src/)
target_link_libraries(island-game libglew_static glfw glm soil2)
target_compile_features(island-game PUBLIC cxx_std_17)
