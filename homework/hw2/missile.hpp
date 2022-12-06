#pragma once

#include <ctime>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <camera.hpp>

using namespace glm;

//снаряд/оружие
//выпускается в направлении камеры и летит 
//пока не улетит слишком далеко или пока не врежется в врага
//тогда оба исчезают

class Missile {
public:
	Missile(const Camera& camera);
	void drawMissile(GLuint programID, GLuint MatrixID) const;
	void updatePosition();
	bool isExist() const;
	void markUnusable();
	glm::vec3 getPosition() const;

	static void init();
	static void freeResources();
	static GLuint VAO_, VBO_;
	static GLfloat vertex_[9];
	static float move_speed_;
	static float max_lifetime_;

private:
	GLfloat getLifetime();

	const glm::vec3 start_pos_;
	const glm::vec3 front_;
	const glm::vec3 up_;

	const Camera& camera_;
	glm::vec3 cur_pos_;
	GLfloat birth_time_; //время создания объекта 
	bool is_usable_;
};