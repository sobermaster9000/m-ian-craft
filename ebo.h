#ifndef EBO_CLASS_H

#define EBO_CLASS_H

#include <vector>
#include <glad/glad.h>

class EBO {
    public:
    GLuint id;
    EBO(std::vector<GLuint>& indices);

    void bind();
    void unbind();
    void destroy();
};

#endif