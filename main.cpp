#include <cmath>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <sstream>

#include "chunk.h"
#include "PerlinNoise.hpp"

#define WIDTH 600
#define HEIGHT 600
#define BUFFER_SIZE 1024

#define CHUNK_SIZE 16
#define RENDER_DIST 8

unsigned int seed = 69420u;

const std::unordered_map<std::string,std::pair<float,float>> texCoordsMapping = {
    {"grass-top", std::pair<float,float>{0.0f,0.75f}},
    {"grass-side", std::pair<float,float>{0.0f,0.5f}},
    {"grass-bottom", std::pair<float,float>{0.0f,0.25f}},
    {"dirt-top", std::pair<float,float>{0.0f,0.25f}},
    {"dirt-side", std::pair<float,float>{0.0f,0.25f}},
    {"dirt-bottom", std::pair<float,float>{0.0f,0.25f}},
    {"sand-top", std::pair<float,float>{0.0f,0.0f}},
    {"sand-side", std::pair<float,float>{0.0f,0.0f}},
    {"sand-bottom", std::pair<float,float>{0.0f,0.0f}},
    {"stone-top", std::pair<float,float>{0.25f,0.75f}},
    {"stone-side", std::pair<float,float>{0.25f,0.75f}},
    {"stone-bottom", std::pair<float,float>{0.25f,0.75f}},
    {"bedrock-top", std::pair<float,float>{0.25f,0.5f}},
    {"bedrock-side", std::pair<float,float>{0.25f,0.5f}},
    {"bedrock-bottom", std::pair<float,float>{0.25f,0.5f}},
    {"wood-top", std::pair{0.25f,0.0f}},
    {"wood-side", std::pair<float,float>{0.25f,0.25f}},
    {"wood-bottom", std::pair{0.25f,0.0f}},
    {"leaves-top", std::pair<float,float>{0.5f,0.75f}},
    {"leaves-side", std::pair<float,float>{0.5f,0.75f}},
    {"leaves-bottom", std::pair<float,float>{0.5f,0.75f}},
    {"water", std::pair<float,float>{0.5f,0.5f}}
};

enum BlockFace {
    FRONT,
    RIGHT,
    BACK,
    LEFT,
    TOP,
    BOTTOM
};

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

void addBlockFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, BlockFace face, std::string blockFaceTexture, float x, float y, float z) {
    float u = texCoordsMapping.at(blockFaceTexture).first;
    float v = texCoordsMapping.at(blockFaceTexture).second;

    switch (face) {
        case FRONT:
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;

        case RIGHT:
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;

        case BACK:
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;

        case LEFT:
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;

        case TOP:
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 1.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;

        case BOTTOM:
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + 0.5f),       glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v)});
        vertices.push_back(Vertex{glm::vec3(x + 0.5f,   y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u + 0.25f, v + 0.25f)});
        vertices.push_back(Vertex{glm::vec3(x + -0.5f,  y + 0.0f,   z + -0.5f),      glm::vec3(1.0f,   1.0f,   1.0f),       glm::vec2(u, v + 0.25f)});
        break;
    }

    int verticesLength = vertices.size();
    indices.push_back(verticesLength-4);
    indices.push_back(verticesLength-3);
    indices.push_back(verticesLength-2);
    indices.push_back(verticesLength-4);
    indices.push_back(verticesLength-2);
    indices.push_back(verticesLength-1);
}

float randomSeedProbability(float x, float y, float z) {
    std::stringstream stream;
    stream << seed << x << y << z;
    std::hash<std::string> hasher;
    std::size_t hashValue = hasher(stream.str());
    return (float)(hashValue % 100) / 100.0f;
}

std::string getTransitionBlock(float x, float y, float z, int transitionStart, int transitionEnd, std::string topBlock, std::string bottomBlock) {
    float transitionCutoff = 0.8f * ((y - transitionStart) / (transitionEnd - transitionStart));
    return (randomSeedProbability(x, y, z) < transitionCutoff ? topBlock : bottomBlock);
}

float getPerlinHeight(int x, int z, siv::PerlinNoise& perlinNoise) {
    return (float)(int)(pow(perlinNoise.octave2D_01((float)x * 0.005f, (float)z * 0.005f, 7), 2.0f) * 100.0f);
}

void generateChunkMeshData(int chunkStartX, int chunkStartZ, siv::PerlinNoise& perlinNoise, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
    std::string currentTexture;

    for (int x = chunkStartX; x < chunkStartX + CHUNK_SIZE; x++) {
        for (int z = chunkStartZ; z < chunkStartZ + CHUNK_SIZE; z++) {

            float y = getPerlinHeight(x, z, perlinNoise);
            float _y = y;
            bool addedFace = true;

            while (addedFace) {
                if (_y > 60)
                    currentTexture = "stone";
                else if (_y > 50)
                    // todo: fix issue where transition block is grass and is not the top block
                    currentTexture = getTransitionBlock(x, _y, z, 50, 60, "stone", "grass");
                else if (_y < 5)
                    currentTexture = "sand";
                else if (_y == y)
                    currentTexture = "grass";
                else
                    currentTexture = "dirt";

                addedFace = false;
                if (getPerlinHeight(x, z-1, perlinNoise) < _y) {
                    // add front face
                    addBlockFace(vertices, indices, FRONT, currentTexture + "-side", x, _y, z);
                    addedFace = true;
                }
                if (getPerlinHeight(x, z+1, perlinNoise) < _y) {
                    // add back face
                    addBlockFace(vertices, indices, BACK, currentTexture + "-side", x, _y, z);
                    addedFace = true;
                }
                if (getPerlinHeight(x+1, z, perlinNoise) < _y) {
                    // add right face
                    addBlockFace(vertices, indices, RIGHT, currentTexture + "-side", x, _y, z);
                    addedFace = true;
                }
                if (getPerlinHeight(x-1, z, perlinNoise) < _y) {
                    // add left face
                    addBlockFace(vertices, indices, LEFT, currentTexture + "-side", x, _y, z);
                    addedFace = true;
                }
                if (_y == y) {
                    // add top face
                    addBlockFace(vertices, indices, TOP, currentTexture + "-top", x, _y, z);
                    addedFace = true;
                }
                _y--;
            }
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

//********** for collisions */

siv::PerlinNoise* perlinNoisePtr = nullptr;

bool hasBlock(float x, float y, float z) {
    // maybe add check if perlinNoisePtr is still nullptr
    return getPerlinHeight(x, z, *perlinNoisePtr) >= y;
}

class Player : public Camera {
    public:
    float boundingBoxMinX;
    float boundingBoxMinY;
    float boundingBoxMinZ;
    float boundingBoxSizeX = 0.8;
    float boundingBoxSizeY = 1.8;
    float boundingBoxSizeZ = 0.8;

    Player(int width, int height, glm::vec3 position) : Camera(width, height, position) {
        updateBoundingBoxPos();
    }

    void updateBoundingBoxPos() {
        boundingBoxMinX = position.x - 0.5f;
        boundingBoxMinY = position.y - 1.0f;
        boundingBoxMinZ = position.z - 0.5f;
    }

    void inputs(GLFWwindow* window) {
        std::vector<glm::vec3> collisionCandidateBlocks;
        for (int x = (int)position.x - 1; x <= (int)position.x + 1; x++) {
            for (int y = (int)position.y - 2; y <= (int)position.y + 1; y++) {
                for (int z = (int)position.z - 1; z <= (int)position.z + 1; z++) {
                    if (hasBlock(x, y, z))
                        collisionCandidateBlocks.push_back(glm::vec3(x, y, z));
                }
            }
        }

        glm::vec3 delta = glm::vec3(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            delta += speed * glm::vec3(orientation.x, 0.0f, orientation.z);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            delta -= speed * glm::normalize(glm::cross(orientation, up));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            delta -= speed * glm::vec3(orientation.x, 0.0f, orientation.z);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            delta += speed * glm::normalize(glm::cross(orientation, up));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            delta += speed * up;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            delta -= speed * up;
        }

        // todo: fix collision checking

        position.x += delta.x;
        for (auto blockPos: collisionCandidateBlocks) {
            if (boundingBoxMinX < blockPos.x + 0.5f && boundingBoxMinX + boundingBoxSizeX > blockPos.x - 0.5f) {
                // position.x -= (delta.x + (delta.x < 0 ? -0.1f : 0.1f));
                position.x = (delta.x < 0 ? blockPos.x + 0.5f + boundingBoxSizeX : blockPos.x - 0.5f - boundingBoxSizeX);
            }
        }

        position.y += delta.y;
        for (auto blockPos: collisionCandidateBlocks) {
            if (boundingBoxMinY < blockPos.y + 0.5f && boundingBoxMinY + boundingBoxSizeY > blockPos.y - 0.5f) {
                // position.y -= (delta.y + (delta.y < 0 ? -0.1f : 0.1f));
                position.y = (delta.y < 0 ? blockPos.y + 0.5f + boundingBoxSizeY : blockPos.y - 0.5f - boundingBoxSizeY);
            }
        }

        position.z += delta.z;
        for (auto blockPos: collisionCandidateBlocks) {
            if (boundingBoxMinZ < blockPos.z + 0.5f && boundingBoxMinZ + boundingBoxSizeZ > blockPos.z - 0.5f) {
                // position.z -= (delta.z + (delta.z < 0 ? -0.1f : 0.1f));
                position.z = (delta.z < 0 ? blockPos.z + 0.5f + boundingBoxSizeZ : blockPos.z - 0.5f - boundingBoxSizeZ);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            speed = 0.5f;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // so that the mouse is recentered every click, preventing abrupt jumping
            if (firstClick) {
                glfwSetCursorPos(window, (width / 2), (height / 2));
                firstClick = false;
            }

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // mouseY in rotationX because rotation around X axis
            // mouseX in rotationY because rotation around Y axis
            float rotationX = sensitivity * (float)(mouseY - (height/2)) / height;
            float rotationY = sensitivity * (float)(mouseX - (width/2)) / width;

            // for clamping camera rotation
            glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotationX), glm::normalize(glm::cross(orientation, up)));
            if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
                orientation = newOrientation;
            }
            orientation = glm::rotate(orientation, glm::radians(-rotationY), up);

            glfwSetCursorPos(window, (width / 2), (height / 2));
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }

        updateBoundingBoxPos();
    }
};

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
        Texture("textures/alpha-atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
    };

    // initialization for chunk data generation
    int playerChunkX, playerChunkZ;
    std::unordered_set<std::pair<int,int>, PairHash, PairEqual> chunkCoords;
    std::queue<std::pair<int,int>> chunksToRender, chunksToDelete;

    siv::PerlinNoise perlinNoise{siv::PerlinNoise::seed_type{seed}};
    perlinNoisePtr = &perlinNoise;
    std::vector<Chunk> chunks;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    // enable depth testing for textures
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 5.0f, 2.0f));
    Player player(WIDTH, HEIGHT, glm::vec3(0.0f, 100.0f, 0.0f));

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
        // camera.inputs(window);
        player.inputs(window);
        // apply camera matrix transformations
        // camera.updateMatrix(90.0f, 0.1f, 200.0f, 1.0f);
        player.updateMatrix(90.0f, 0.1f, 200.0f, 1.0f);
        
        // draw meshes
        for (Chunk& chunk: chunks)
            chunk.draw(shader, player);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // for chunk loading
        playerChunkX = (int)player.position.x/CHUNK_SIZE*CHUNK_SIZE;
        playerChunkZ = (int)player.position.z/CHUNK_SIZE*CHUNK_SIZE;
        for (int x = playerChunkX-RENDER_DIST*CHUNK_SIZE; x <= playerChunkX+RENDER_DIST*CHUNK_SIZE; x += CHUNK_SIZE) {
            for (int z = playerChunkZ-RENDER_DIST*CHUNK_SIZE; z <= playerChunkZ+RENDER_DIST*CHUNK_SIZE; z += CHUNK_SIZE) {
                // distance check
                if ((int)sqrt((pow(x-playerChunkX,2) + pow(z-playerChunkZ,2))) > RENDER_DIST*CHUNK_SIZE)
                    continue;
                // check if chunk already exists
                if (chunkCoords.find(std::pair<int,int>{x,z}) != chunkCoords.end())
                    continue;
                // chunk data generation
                chunkCoords.insert(std::pair<int,int>{x,z});
                vertices.clear();
                indices.clear();
                generateChunkMeshData(x, z, perlinNoise, vertices, indices);
                chunks.push_back(Chunk(x, z, vertices, indices, textures));
            }
        }

        // for chunk unloading
        for (auto it = chunkCoords.begin(); it != chunkCoords.end(); ) {
            int chunkX = it->first;
            int chunkZ = it->second;
            // distance check
            if ((int)sqrt((pow(chunkX-playerChunkX,2) + pow(chunkZ-playerChunkZ,2))) > RENDER_DIST*CHUNK_SIZE) {
                // remove from chunkCoords
                it = chunkCoords.erase(it);
                // remove from chunks vector
                for (auto chunkIt = chunks.begin(); chunkIt != chunks.end(); chunkIt++) {
                    if (chunkIt->chunkX == chunkX && chunkIt->chunkZ == chunkZ) {
                        chunks.erase(chunkIt);
                        break;
                    }
                }
            } else {
                it++;
            }
        }

        // debug
        std::cout << player.position.x << " ";
        std::cout << player.position.y << " ";
        std::cout << player.position.z << std::endl;
    }
    /********** End of Main Loop **********/
    
    /********** Cleanup **********/
    shader.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}