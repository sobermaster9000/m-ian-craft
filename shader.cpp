#include "shader.h"

std::string readShaderFile(const char* filename) {
    std::ifstream shaderFile(filename);
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    std::string shaderSource = buffer.str();
    shaderFile.close();
    return shaderSource.c_str();
}

Shader::Shader(const char* vertexShaderFile, const char* fragmentShaderFile) {
    std::string vertexShaderString = readShaderFile(vertexShaderFile);
    std::string fragmentShaderString = readShaderFile(fragmentShaderFile);
    const char* vertexShaderSource = vertexShaderString.c_str();
    const char* fragmentShaderSource = fragmentShaderString.c_str();

    // for printing our errors
    int success; char infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Error compiling vertex shader\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Error compiling vertex shader\n" << infoLog << std::endl;
    }

    // linking shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error linking shader program\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::useShader() {
    glUseProgram(shaderProgram);
}

void Shader::deleteShader() {
    glDeleteProgram(shaderProgram);
}