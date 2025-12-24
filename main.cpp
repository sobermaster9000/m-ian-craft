#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
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

        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        -0.5f,  1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        0.5f,   1.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        0.5f,   1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        -0.5f,  1.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  1.0f,

        -0.5f,  0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       0.0f,  0.0f,
        0.5f,   0.0f,   0.5f,       1.0f,   1.0f,   1.0f,       1.0f,  0.0f,
        0.5f,   0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       1.0f,  1.0f,
        -0.5f,  0.0f,   -0.5f,      1.0f,   1.0f,   1.0f,       0.0f,  1.0f
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

    //declare ang generate buffers
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind and initialize buffer data
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // for vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    // for color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // for texCoords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    // unbind buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
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
        camera.matrix(45.0f, 0.1f, 100.0f, shader, "cameraMatrix");
        
        // draw vertices
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    /********** End of Main Loop **********/
    
    /********** Cleanup **********/
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    shader.deleteShader();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}