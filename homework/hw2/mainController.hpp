#pragma once
#include<missile.hpp>
#include<enemy.hpp>
#include<camera.hpp>

#include <shader.hpp>

const double SPACINGBETWEENENEMIES = 4.0;

class MainController {
public:
	MainController();

	void init(GLuint programID, GLuint matrixID);
	void addNewEnemy();
	void shoot();
	void updateObject();
	Camera& getCamera();
	double getSpacingBetweenEnemies();
	~MainController();

private:
	void updateMisselesPosition();
	void killEnemies();
	void deleteKilledEnemies();
	void deleteUnusedMissiles();

	Camera camera;
	Camera::Gunsight gs;
	std::list<Missile> missiles; //текущие летящие снаряды
	std::list<Enemy> enemies; //текущие живые враги

	GLuint programID_;
	GLuint matrixID_;

	double spacing_between_enemies;
};