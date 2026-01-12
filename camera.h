#ifndef CAMERA_CLASS_H

#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"

class Camera {
    public:

    glm::vec3 position;
    glm::vec3 orientation = glm::vec3(0, 0, -1.0f);
    glm::vec3 up = glm::vec3(0, 1.0f, 0);

    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    bool firstClick = true;

    int width;
    int height;

    float speed = 1.0f;
    float sensitivity = 100.0f;

    Camera(int width, int height, glm::vec3 position);

    void updateMatrix(float FOVdeg, float nearPlane, float farPlane, float scale);
    void matrix(Shader& shader, const char* uniform);
    void inputs(GLFWwindow* window);
};

#endif