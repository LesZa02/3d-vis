#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<list>

#include<missile.hpp>

//класс врагов
//с позацией и оринтацией в пространстве
//возникают в ранжомных точках окрестности камеры

class Enemy {
public:
	Enemy(const Camera& camera);
	void drawEnemy(GLuint programID, GLuint MatrixID) const;
	bool isAlive() const;
	void markAsDead();
	glm::vec3 getPosition();

	static void init();
	static void freeResources();
	static float getSafeRadius();
	static GLfloat safe_radius_;
	static GLuint VAO_, VBO_, EBO_;
	static GLfloat vertex_[12];
	static GLuint indices[12];
	static GLint max_dist_;
	static GLint min_dist_;

private:
	void setRandomPosition();

	const Camera& camera_;
	glm::vec3 pos_;
	glm::mat4 model_;
	bool is_alive_;
};