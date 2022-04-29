#include <SOIL2.h>
#include "Texture.h"

namespace  {
    void invertY(unsigned char* image, int width, int height, int channels) {
        for (int j = 0; j * 2 < height; ++j) {
            unsigned int index1 = j * width * channels;
            unsigned int index2 = (height - 1 - j) * width * channels;
            for (int i = 0; i < width * channels; i++) {
                unsigned char temp = image[index1];
                image[index1] = image[index2];
                image[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }

    void invertX(unsigned char* image, int width, int height, int channels) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; 2 * j < width; ++j) {
                for (int k = 0; k < channels; ++k) {
                    int j_1 = j;
                    int j_2 = (width - 1 - j);
                    unsigned char tmp = image[i * width * channels + j_1 * channels + k];
                    image[i * width * channels + j_1 * channels + k] =  image[i * width * channels + j_2 * channels + k];
                    image[i * width * channels + j_2 * channels + k] = tmp;
                }
            }
        }
    }

}

bool load_image(const std::string &filename, bool srgb, int &width, int &height, int &channels,
                GLint &internalFormat, GLenum &format, unsigned char* &data) {
    data = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if (!data) {
        LOGE("SOIL loading error: %s", SOIL_last_result());
        return false;
    }
    ::invertY(data, width, height, channels);
    internalFormat = (channels == 4) ? (srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8) :
                     (channels == 3 ? (srgb ? GL_SRGB8: GL_RGB8) : GL_R8);
    LOGI("Loaded %s channels %d", filename.c_str(), channels);
    format = (channels == 4) ? GL_RGBA : (channels == 3 ? GL_RGB : GL_R);
    return true;
}

TexturePtr Texture::load_texture(const std::string &filename, bool srgb) {
    int width, height, channels;
    GLint internalFormat;
    GLenum format;
    unsigned char *image;
    if (!load_image(filename, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }

    TexturePtr texture = std::make_shared<Texture>(GL_TEXTURE_2D);
    texture->setTexImage2D(0, internalFormat, width, height, format, GL_UNSIGNED_BYTE, image);
    texture->generateMipmaps();

    SOIL_free_image_data(image);

    return texture;
}

TexturePtr CubeTexture::load_cube_texture(const std::string &right, const std::string &left, const std::string &top,
                                          const std::string &bottom, const std::string &back, const std::string &front,
                                          bool srgb) {
    LOGI("Error before load_cube_texture: %d", glGetError());
    LOGI("Error before load_cube_texture: %d", glGetError());

    TexturePtr texture = std::make_shared<CubeTexture>();
    int width, height, channels;
    GLint internalFormat;
    GLenum format;
    unsigned char *image;


    if (!load_image(right, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    if (!load_image(left, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);



    if (!load_image(top, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    ::invertY(image, width, height, channels);
    ::invertX(image, width, height, channels);
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    if (!load_image(bottom, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    ::invertY(image, width, height, channels);
    ::invertX(image, width, height, channels);
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);



    if (!load_image(back, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    if (!load_image(front, srgb, width, height, channels, internalFormat, format, image)) {
        return nullptr;
    }
    texture->setTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat,
                           width, height, format, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);


    LOGI("Error after load_cube_texture: %d", glGetError());
    return texture;
}
