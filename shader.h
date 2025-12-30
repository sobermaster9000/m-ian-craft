#ifndef SHADER_CLASS_H

#define SHADER_CLASS_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

std::string readShaderFile(const char* filename);

class Shader {
    public:
    GLuint id;
    Shader(const char* vertexShaderFile, const char* fragmentShaderFile);
    void use();
    void destroy();
};

#endif