#include <mainController.hpp>

MainController::MainController() :
	programID_(0), matrixID_(0), spacing_between_enemies(SPACINGBETWEENENEMIES) {
}

void MainController::init(GLuint programID, GLuint matrixID) {
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	Enemy::init(); //создаем контекст для врагов
	Missile::init();
	gs.init();

	programID_ = programID;
	matrixID_ = matrixID;
}

void MainController::addNewEnemy() {
	if (enemies.size() < 10) {
		enemies.emplace_back(camera);
	}
}

void MainController::shoot() {
	missiles.emplace_back(camera);
}

void MainController::updateObject() {
	updateMisselesPosition();
	killEnemies();
	deleteKilledEnemies();
	deleteUnusedMissiles();

	gs.drawGunsight(programID_, matrixID_);

	for (auto& e : enemies) {
		e.drawEnemy(programID_, matrixID_);
	}
	for (auto& m : missiles) {
		m.drawMissile(programID_, matrixID_);
	}
}

Camera& MainController::getCamera() {
	return camera;
}

double MainController::getSpacingBetweenEnemies() {
	return spacing_between_enemies;
}

MainController::~MainController() {
	Enemy::freeResources(); //удаляем контекст для врагов
	Missile::freeResources();
	gs.freeResources();
}

///////////////////////
void MainController::updateMisselesPosition() {
	for (auto& m : missiles) {
		m.updatePosition();
	}
}

void MainController::killEnemies() {
	for (auto& e : enemies) {
		for (auto& m : missiles) {
			if (glm::length(e.getPosition() - m.getPosition()) < e.getSafeRadius()) {
				e.markAsDead();
				m.markUnusable();
			}
		}
	}
}

void MainController::deleteKilledEnemies() {
	for (auto eit = enemies.begin(); eit != enemies.end();) {
		if (!eit->isAlive()) {
			eit = enemies.erase(eit);
		}
		else {
			eit++;
		}
	}
}

void MainController::deleteUnusedMissiles() {
	for (auto mit = missiles.begin(); mit != missiles.end();) {
		if (!mit->isExist()) {
			mit = missiles.erase(mit);
		}
		else {
			mit++;
		}
	}
}