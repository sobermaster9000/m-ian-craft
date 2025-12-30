#ifndef VBO_CLASS_H

#define VBO_CLASS_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

typedef struct{
    glm::vec3 position;
    // glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoords;
} Vertex;

class VBO {
    public:
    GLuint id;
    VBO(std::vector<Vertex>& vertices);

    void bind();
    void unbind();
    void destroy();
};

#endif