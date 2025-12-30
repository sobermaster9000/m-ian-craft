#include "vao.h"

VAO::VAO() {
    glGenVertexArrays(1, &id);
}

void VAO::linkAttrib(VBO vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset) {
    vbo.bind();

    glVertexAttribPointer(layout, components, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);

    // // for vertex attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    // glEnableVertexAttribArray(0);
    
    // // for color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(1);
    
    // // for texCoords attribute
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(2);

    vbo.unbind();
}

void VAO::bind() {
    glBindVertexArray(id);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

void VAO::destroy() {
    glDeleteVertexArrays(1, &id);
}