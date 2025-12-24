#include "camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
    Camera::width = width;
    Camera::height = height;
    Camera::position = position;
}

void Camera::matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
    // initialize vertices
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    // make camera look forward
    viewMatrix = glm::lookAt(position, position + orientation, up);
    projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)(width/height), nearPlane, farPlane);

    // update visual transformations in shader
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, uniform), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));
}

void Camera::inputs(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += speed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= speed * glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= speed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += speed * glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += speed * up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        position -= speed * up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed = 0.05f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        speed = 0.01f;
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

        // adjusts rotation of camera based on mouse movement,
        // but i dont fully understand this yet
        float rotationX = sensitivity * (float)(mouseY - (width/2)) / width;
        float rotationY = sensitivity * (float)(mouseX - (width/2)) / width;

        // i dont fully understand what this is for
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
}