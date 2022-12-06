#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const GLuint WIDTH = 1200, HEIGHT = 800;
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.15f;
const GLfloat ZOOM = 45.0f;
const GLfloat ANGLE = 45.0f;
const GLfloat PROPORTION = (GLfloat)WIDTH / (GLfloat)HEIGHT;
const GLint RESERVED_ANGLE = 5;

class Camera
{
public:
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class Gunsight {
    public:
        Gunsight();
        void init();
        void drawGunsight(GLuint programID, GLuint MatrixID) const;
        void freeResources();
        
    private:
        GLuint VAO_, VBO_;
        GLfloat vertex_[12] = {
        -0.15f, 0.0f, 0.0f, 
        0.15f, 0.0f, 0.0f,
        0.0f, -0.2f, 0.0f,
        0.0f, 0.2f, 0.0f};
    };

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
        GLfloat yaw = YAW, GLfloat pitch = PITCH);
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, 
        GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch); 

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionViewMatrix() const;
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    glm::vec3 GetCameraPos() const;
    glm::vec3 GetCameraFront() const;
    glm::vec3 GetCameraUp() const;
    GLfloat GetCameraAngle() const;
    GLfloat GetCameraProportion() const;
    GLint GetReservedAngle() const;

private:
    void updateCameraVectors();

    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 Up;

    glm::vec3 world_up_;

    GLfloat yaw_;
    GLfloat pitch_;

    GLfloat movement_speed_;
    GLfloat mouse_sensitivity_;

    GLfloat camera_angle_;
    GLfloat camera_proportion_;
    GLint angle_reserve_;
};