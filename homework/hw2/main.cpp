#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include<list>

#include <shader.hpp>
#include <texture.hpp>
#include <enemy.hpp>
#include <missile.hpp>
#include <mainController.hpp>


MainController m_controller;

//Global
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
int prev_state = GLFW_RELEASE;
bool keys[1024];
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

//Missile
float Missile::move_speed_ = 6;
float Missile::max_lifetime_ = 10; //s
GLuint Missile::VAO_ = 0;
GLuint Missile::VBO_ = 0;
GLfloat Missile::vertex_[] = { -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f };

//Enemy
float Enemy::safe_radius_ = 0.5;
GLuint Enemy::VAO_ = 0;
GLuint Enemy::VBO_ = 0;
GLuint Enemy::EBO_ = 0;
GLfloat Enemy::vertex_[] = {-0.5f, 0.0f, -0.5f, 
                            0.5f, 0.0f, -0.5f,
                            0.0f,  0.0f, 0.5f,
                            0.0f,  0.5f, 0.0f };
GLuint Enemy::indices[] = { 0, 1, 2,
                            0, 1, 3,
                            1, 2, 3,
                            2, 0, 3 };
GLint Enemy::max_dist_ = 30;
GLint Enemy::min_dist_ = 5;

void do_movement()
{
    if (keys[GLFW_KEY_W])
        m_controller.getCamera().ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        m_controller.getCamera().ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        m_controller.getCamera().ProcessKeyboard(Camera::LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        m_controller.getCamera().ProcessKeyboard(Camera::RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    m_controller.getCamera().ProcessMouseMovement(xoffset, yoffset);
}

int setUpEnvironment(GLFWwindow*& window, GLuint& programID, GLuint& MatrixID) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //указание на версию 3.*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //указание на версию *.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // ¬ключить тест глубины
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //альфа-канальность
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    MatrixID = glGetUniformLocation(programID, "MVP"); 

    return 0;
}

int main(void)
{
    //инициализаци€ всего
    GLFWwindow* window = nullptr;
	GLuint VertexArrayID;
	GLuint programID;
    GLuint matrixID;

    if (setUpEnvironment(window, programID, matrixID) != 0) {
        return -1;
    }

    m_controller.init(programID, matrixID);

    double prev_time = 0;
    bool was_added_at_this_second = false;
    
    //игровой цикл
    do {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if ((currentFrame - prev_time) > m_controller.getSpacingBetweenEnemies() || !was_added_at_this_second) {
            m_controller.addNewEnemy();
            was_added_at_this_second = true;
            prev_time = currentFrame;
        }

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state != prev_state) {
            prev_state = state;
            if (state == GLFW_PRESS) {
                m_controller.shoot();
            }
        }
        m_controller.updateObject();
        
        glfwSwapBuffers(window);
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

    glDeleteProgram(programID);
    glfwTerminate();
    return 0;
}