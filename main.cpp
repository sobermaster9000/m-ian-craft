#include <cmath>
#include <unordered_set>
#include <queue>

#include "mesh.h"
#include "PerlinNoise.hpp"

#define WIDTH 600
#define HEIGHT 600
#define BUFFER_SIZE 1024

#define CHUNK_SIZE 16
#define RENDER_DIST 5

// for hashing pairs in unordered_set
struct PairHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

// for comparing pairs in unordered_set
struct PairEqual {
    template <class T1, class T2>
    bool operator() (const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

float getPerlinHeight(int x, int z, siv::PerlinNoise& perlinNoise) {
    return (float)(int)(perlinNoise.octave2D_01((float)x * 0.03f, (float)z * 0.03f, 5) * 9.5f);
}

void generateChunkMeshData(int chunkStartX, int chunkStartZ, siv::PerlinNoise& perlinNoise, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
    int verticesLength;
    for (int x = chunkStartX; x < chunkStartX + CHUNK_SIZE; x++) {
        for (int z = chunkStartZ; z < chunkStartZ + CHUNK_SIZE; z++) {
            float y = getPerlinHeight(x, z, perlinNoise);
            if (getPerlinHeight(x, z-1, perlinNoise) < y) {
                // add front face
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  1.0f)});
                verticesLength = vertices.size();
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-3);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-1);
            }
            if (getPerlinHeight(x, z+1, perlinNoise) < y) {
                // add back face
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)});
                verticesLength = vertices.size();
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-3);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-1);
            }
            if (getPerlinHeight(x+1, z, perlinNoise) < y) {
                // add right face
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)});
                vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)});
                verticesLength = vertices.size();
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-3);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-1);
            }
            if (getPerlinHeight(x-1, z, perlinNoise) < y) {
                // add left face
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  0.5f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  1.0f)});
                vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)});
                verticesLength = vertices.size();
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-3);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-4);
                indices.push_back(verticesLength-2);
                indices.push_back(verticesLength-1);
            }
            // add top face
            vertices.push_back(Vertex{glm::vec3((float)x + -0.5f,  (float)y + 1.0f,   (float)z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.0f)});
            vertices.push_back(Vertex{glm::vec3((float)x + 0.5f,   (float)y + 1.0f,   (float)z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.0f)});
            vertices.push_back(Vertex{glm::vec3((float)x + 0.5f,   (float)y + 1.0f,   (float)z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)});
            vertices.push_back(Vertex{glm::vec3((float)x + -0.5f,  (float)y + 1.0f,   (float)z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)});
            verticesLength = vertices.size();
            indices.push_back(verticesLength-4);
            indices.push_back(verticesLength-3);
            indices.push_back(verticesLength-2);
            indices.push_back(verticesLength-4);
            indices.push_back(verticesLength-2);
            indices.push_back(verticesLength-1);
        }
    }
}

void makeBlockVertices(std::vector<Vertex>& vertices, float x, float y, float z) {
                        // vertices                                              colors                                 texCoords //
    // front face
    vertices[0] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)};
    vertices[1] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[2] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)};
    vertices[3] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  1.0f)};

    // right face
    vertices[4] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[5] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)};
    vertices[6] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)};
    vertices[7] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  1.0f)};

    // back face
    vertices[8] = Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  0.5f)};
    vertices[9] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)};
    vertices[10] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)};
    vertices[11] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.5f,  1.0f)};

    // left face
    vertices[12] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  0.5f)};
    vertices[13] = Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  0.5f)};
    vertices[14] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(1.0f,  1.0f)};
    vertices[15] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.75f,  1.0f)};

    // top face
    vertices[16] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.0f)};
    vertices[17] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.0f)};
    vertices[18] = Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.25f,  0.5f)};
    vertices[19] = Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(0.0f,  0.5f)};

    // bottom face
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

    std::vector<Texture> textures{
        Texture("textures/grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
    };

    int playerChunkX, playerChunkZ;
    std::unordered_set<std::pair<int,int>, PairHash, PairEqual> chunkCoords;
    std::queue<std::pair<int,int>> chunksToRender, chunksToDelete;

    siv::PerlinNoise perlinNoise{siv::PerlinNoise::seed_type{69420u}};
    std::vector<Mesh> meshes;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    // for (int x = -RENDER_DIST*CHUNK_SIZE; x <= RENDER_DIST*CHUNK_SIZE; x += CHUNK_SIZE) {
    //     for (int z = -RENDER_DIST*CHUNK_SIZE; z <= RENDER_DIST*CHUNK_SIZE; z += CHUNK_SIZE) {
    //         if ((int)sqrt((pow(x,2) + pow(z,2))) > RENDER_DIST*CHUNK_SIZE)
    //             continue;

    //         vertices.clear();
    //         indices.clear();
    //         generateChunkMeshData(x, z, perlinNoise, vertices, indices);
    //         meshes.push_back(Mesh(vertices, indices, textures));
    //     }
    // }

    // enable depth testing for textures
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 5.0f, 2.0f));

    // for FPS counter
    double previousTime;
    double currentTime;
    double timeDifference;
    unsigned int frameCount = 0;

    /********** Main Loop **********/
    while (!glfwWindowShouldClose(window)) {
        // for FPS counter
        currentTime = glfwGetTime();
        timeDifference = currentTime - previousTime;
        frameCount++;

        if (timeDifference >= 1.0) {
            std::string FPS = std::to_string(1.0 / timeDifference * frameCount);
            std::string msPerFrame = std::to_string(timeDifference / frameCount * 1000.0);
            std::string newTitle = "M-Ian-Craft Beta - " + FPS + " FPS / " + msPerFrame + " ms";
            glfwSetWindowTitle(window, newTitle.c_str());
            frameCount = 0;
            previousTime = currentTime;
        }

        processInput(window);
        
        // update background with color
        glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // for camera movement
        camera.inputs(window);
        // apply camera matrix transformations
        camera.updateMatrix(90.0f, 0.1f, 100.0f, 1.0f);
        
        // draw meshes
        for (Mesh& mesh: meshes)
            mesh.draw(shader, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // for chunk loading
        playerChunkX = (int)camera.position.x/CHUNK_SIZE*CHUNK_SIZE;
        playerChunkZ = (int)camera.position.z/CHUNK_SIZE*CHUNK_SIZE;
        for (int x = playerChunkX-RENDER_DIST*CHUNK_SIZE; x <= playerChunkX+RENDER_DIST*CHUNK_SIZE; x += CHUNK_SIZE) {
            for (int z = playerChunkZ-RENDER_DIST*CHUNK_SIZE; z <= playerChunkZ+RENDER_DIST*CHUNK_SIZE; z += CHUNK_SIZE) {
                if ((int)sqrt((pow(x-playerChunkX,2) + pow(z-playerChunkZ,2))) > RENDER_DIST*CHUNK_SIZE)
                    continue;
                if (chunkCoords.find(std::pair<int,int>{x,z}) != chunkCoords.end())
                    continue;
                chunkCoords.insert(std::pair<int,int>{x,z});
                vertices.clear();
                indices.clear();
                generateChunkMeshData(x, z, perlinNoise, vertices, indices);
                meshes.push_back(Mesh(vertices, indices, textures));
            }
        }

        std::cout << camera.position.x << " ";
        std::cout << camera.position.y << " ";
        std::cout << camera.position.z << std::endl;
    }
    /********** End of Main Loop **********/
    
    /********** Cleanup **********/
    shader.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}