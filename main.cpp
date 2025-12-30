#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"
#include "camera.h"
using namespace std;

#define WIDTH 600
#define HEIGHT 600
#define BUFFER_SIZE 1024

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main() {
    /********** Initialization **********/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M-Ian-Craft Beta", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    // for screen resizeability
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // initialize shaders
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    /********** End of Initialization **********/

    /********** Vertex Data and Buffers **********/
    GLfloat vertices[] = {
        // vertices                 colors                      texCoords //

        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  0.5f,
        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  0.5f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  1.0f,
        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  0.5f,
        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.5f,  0.5f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.5f,  1.0f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  1.0f,

        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.5f,  0.5f,
        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.75f,  0.5f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.75f,  1.0f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.5f,  1.0f,

        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.75f,  0.5f,
        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  0.5f,
        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.75f,  1.0f,

        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  0.0f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.25f,  0.5f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  0.5f,

        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.25f,  0.0f,
        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.5f,  0.0f,
        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.5f,  0.5f,
        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.25f,  0.5f
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23
    };

    VAO vao; vao.bind();
    VBO vbo(vertices, sizeof(vertices));
    EBO ebo(indices, sizeof(indices));

    vao.linkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(0));
    vao.linkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    vao.linkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    vao.unbind();
    vbo.unbind();
    ebo.unbind();
    /********** End of Vertex Data and Buffers **********/

    /********** Loading Texture **********/
    // load texture bytes stb_image
    int textureWidth, textureHeight, textureColorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load("textures/grass.png",
        &textureWidth, &textureHeight, &textureColorChannels, 0);
    
    // declare texture with opengl
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // for maintaining pixels and not blurring them
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // repeat image on s (x) and t (y) axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // initialize opengl texture image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // free and unbind
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // update shader sampler2D with loaded texture
    GLuint texture0Uniform = glGetUniformLocation(shader.shaderProgram, "texture0");
    shader.useShader();
    glUniform1i(texture0Uniform, 0);
    /********** End of Loading Texture **********/

    // enable depth testing for textures
    glEnable(GL_DEPTH_TEST);

    // initialize camera
    Camera camera(WIDTH, HEIGHT, glm::vec3(0, 0, 2.0f));

    /********** Main Loop **********/
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        // update background with color
        glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader and bind texture
        shader.useShader();
        glBindTexture(GL_TEXTURE_2D, texture);

        // for camera movement
        camera.inputs(window);
        // apply camera matrix transformations
        camera.matrix(90.0f, 0.1f, 100.0f, shader, "cameraMatrix");
        
        // draw vertices
        vao.bind();
        ebo.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
        vao.unbind();
        ebo.unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    /********** End of Main Loop **********/
    
    /********** Cleanup **********/
    vao.destroy();
    vbo.destroy();
    ebo.destroy();
    glDeleteTextures(1, &texture);
    shader.deleteShader();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}