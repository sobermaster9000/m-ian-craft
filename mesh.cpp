#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    vao.bind();
    VBO vbo(vertices);
    EBO ebo(indices);

    vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)(0));
    vao.linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
    vao.linkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));

    vao.unbind();
    vbo.unbind();
    ebo.unbind();
}

void Mesh::draw(Shader& shader, Camera& camera) {
    shader.use();
    vao.bind();
    
    unsigned int textureCount = 0;
    for (unsigned int i = 0; i < textures.size(); i++) {
        textures[i].textureUnit(shader, (std::string("texture") + std::to_string(textureCount++)).c_str(), i);
        textures[i].bind();
    }

    camera.matrix(shader, "cameraMatrix");
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}