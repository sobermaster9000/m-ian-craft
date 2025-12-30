#ifndef VAO_CLASS_H

#define VAO_CLASS_H

#include <glad/glad.h>
#include "vbo.h"

class VAO {
    public:
    GLuint id;
    VAO();

    void linkAttrib(VBO& vbo, GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset);
    void bind();
    void unbind();
    void destroy();

};

#endif