#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "texture.h"

Texture::Texture(const char* image, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType) {
    type = textureType;

    // load texture bytes stb_image
    int textureWidth, textureHeight, textureColorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &textureWidth, &textureHeight, &textureColorChannels, 0);
    // std::cout << image << " " << textureWidth << " " << textureHeight << " " << textureColorChannels << std::endl;

    // declare texture with opengl
    glGenTextures(1, &id);
    glActiveTexture(slot);
    glBindTexture(type, id);

    // for maintaining pixels and not blurring them
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // repeat image on s (x) and t (y) axis
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // initialize opengl texture image
    glTexImage2D(type, 0, GL_RGBA, textureWidth, textureHeight, 0, format, pixelType, bytes);
    glGenerateMipmap(type);
    
    // free and unbind
    stbi_image_free(bytes);
    glBindTexture(type, 0);
}

void Texture::textureUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint texture0Uniform = glGetUniformLocation(shader.id, uniform);
    shader.use();
    glUniform1i(texture0Uniform, unit);
}

void Texture::bind() {
    glBindTexture(type, id);
}

void Texture::unbind() {
    glBindTexture(type, 0);
}

void Texture::destroy() {
    glDeleteTextures(1, &id);
}