#include <camera.hpp>


Camera::Gunsight::Gunsight() {}

void Camera::Gunsight::init() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_), vertex_, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Camera::Gunsight::drawGunsight(GLuint programID, GLuint MatrixID) const {
    glm::mat4 model_ = glm::mat4(1.0f);
    glm::mat4 MVP_ = model_;
    glUseProgram(programID);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_[0][0]);
    glBindVertexArray(VAO_);
    glDrawArrays(GL_LINES, 0, 3 * 2);
    glBindVertexArray(0);
}

void Camera::Gunsight::freeResources() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
}

////////////////
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) : 
    front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVTY), 
    camera_angle_(ANGLE), camera_proportion_(PROPORTION), angle_reserve_(RESERVED_ANGLE)
{
    position_ = position;
    world_up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, 
    GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : 
    front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVTY),
    camera_angle_(ANGLE), camera_proportion_(PROPORTION), angle_reserve_(RESERVED_ANGLE)
{
    position_ = glm::vec3(posX, posY, posZ);
    world_up_ = glm::vec3(upX, upY, upZ);
    yaw_ = yaw;
    pitch_ = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, Up);
}

glm::mat4 Camera::GetProjectionViewMatrix() const {
    glm::mat4 projection = glm::perspective(camera_angle_, camera_proportion_, 0.1f, 100.0f);
    return projection * GetViewMatrix();
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = movement_speed_ * deltaTime;
    if (direction == FORWARD)
        position_ += front_ * velocity;
    if (direction == BACKWARD)
        position_ -= front_ * velocity;
    if (direction == LEFT)
        position_ -= glm::normalize(glm::cross(front_, world_up_)) * velocity;
    if (direction == RIGHT)
        position_ += glm::normalize(glm::cross(front_, world_up_)) * velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (constrainPitch)
    {
        if (pitch_ > 70.0f)
            pitch_ = 70.0f;
        if (pitch_ < -70.0f)
            pitch_ = -70.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    Up = glm::normalize(glm::cross(glm::normalize(glm::cross(front_, world_up_)), front_));
}

glm::vec3 Camera::GetCameraPos() const {
    return position_;
}

glm::vec3 Camera::GetCameraFront() const {
    return front_;
}

glm::vec3 Camera::GetCameraUp() const {
    return Up;
}

GLfloat Camera::GetCameraAngle() const {
    return camera_angle_;
}

GLfloat Camera::GetCameraProportion() const {
    return camera_proportion_;
}

GLint Camera::GetReservedAngle() const {
    return angle_reserve_;
}