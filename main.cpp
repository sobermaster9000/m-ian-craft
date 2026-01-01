#include "mesh.h"

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

void makeBlockVertices(std::vector<Vertex>& vertices, float x, float y, float z) {
                        // vertices                                              colors                                 texCoords //

    vertices[0] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)};
    vertices[1] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[2] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)};
    vertices[3] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  1.0f)};

    vertices[4] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[5] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)};
    vertices[6] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)};
    vertices[7] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)};

    vertices[8] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)};
    vertices[9] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)};
    vertices[10] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)};
    vertices[11] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)};

    vertices[12] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)};
    vertices[13] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  0.5f)};
    vertices[14] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  1.0f)};
    vertices[15] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)};

    vertices[16] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.0f)};
    vertices[17] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.0f)};
    vertices[18] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[19] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)};

    vertices[20] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.0f)};
    vertices[21] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.0f)};
    vertices[22] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)};
    vertices[23] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
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
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    // for screen resizeability
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // initialize shaders
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    std::vector<GLuint> indices{
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

    std::vector<Texture> textures{
        Texture("textures/grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
    };

    std::vector<Mesh> meshes;
    std::vector<Vertex> vertices(24);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            makeBlockVertices(vertices, (float)i, 0.0f, (float)j);
            meshes.push_back(Mesh(vertices, indices, textures));
        }
    }

    // enable depth testing for textures
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0, 0, 2.0f));

    /********** Main Loop **********/
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        // update background with color
        glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // for camera movement
        camera.inputs(window);
        // apply camera matrix transformations
        camera.updateMatrix(90.0f, 0.1f, 100.0f);
        
        // draw meshes
        // block.draw(shader, camera);
        for (Mesh& mesh: meshes)
            mesh.draw(shader, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    /********** End of Main Loop **********/
    
    /********** Cleanup **********/
    shader.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}