#include <enemy.hpp>

//получаем рандомный угол по горизонтали в поле зрения (угол обзора 45)
//по вертикали (угол обзора * соотношение сторон)
//рандомное растояние в пределех(10б 20)
//по есть получили направление вектора и его длину
//выбираем рандомный векторкор как ось вращения и рандомный угол, поворачиваем

void Enemy::init() { //привязываем контекст
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glGenBuffers(1, &EBO_);

	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_), vertex_, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Enemy::freeResources() {
	glDeleteVertexArrays(1, &VAO_);
	glDeleteBuffers(1, &VBO_);
	glDeleteBuffers(1, &EBO_);
}

////////////////////////
void Enemy::setRandomPosition() {
	GLfloat dist_hor = rand() % (max_dist_ - min_dist_) + min_dist_;
	GLfloat angle_hor = glm::radians(rand() % int(
		camera_.GetCameraAngle() * camera_.GetCameraProportion() - camera_.GetReservedAngle()) -
		(camera_.GetCameraAngle() * camera_.GetCameraProportion() - camera_.GetReservedAngle()) / 2);

	GLfloat angle_vert = glm::radians(rand() % int(
		camera_.GetCameraAngle() - camera_.GetReservedAngle()) -
		(camera_.GetCameraAngle() - camera_.GetReservedAngle()) / 2);

	//строим вектор до точки расположения
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, angle_hor, camera_.GetCameraUp());
	transform = glm::rotate(transform, angle_vert, 
		glm::cross(glm::vec3(transform * glm::vec4(camera_.GetCameraFront(), 1.0f)), camera_.GetCameraUp()));
	pos_ = glm::vec3(transform * glm::vec4(dist_hor * camera_.GetCameraFront() + camera_.GetCameraPos(), 1.0f));

	//строим матрицу модели
	model_ = glm::translate(model_, pos_); 
	model_ = glm::rotate(model_, glm::radians(rand() / 10.0f), camera_.GetCameraUp());
	model_ = glm::rotate(model_, glm::radians(rand() / 10.0f), glm::vec3(model_ * glm::vec4(camera_.GetCameraFront(), 1.0f)));
	model_ = glm::rotate(model_, glm::radians(rand() / 10.0f), 
		glm::cross(glm::vec3(model_ * glm::vec4(camera_.GetCameraFront(), 1.0f)),
			glm::vec3(model_ * glm::vec4(camera_.GetCameraUp(), 1.0f))));
}

Enemy::Enemy(const Camera& camera) : camera_(camera) {
	is_alive_ = true;
	model_ = glm::mat4(1.0f);
	setRandomPosition();
	std::cout << "ALARM!!!" << "\n";
}

void Enemy::drawEnemy(GLuint programID, GLuint MatrixID) const {
	glm::mat4 MVP_ = camera_.GetProjectionViewMatrix() * model_;
	glUseProgram(programID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_[0][0]);
	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Enemy::isAlive() const {
	return is_alive_;
}

void Enemy::markAsDead() {
	is_alive_ = false;
}

float Enemy::getSafeRadius() {
	return safe_radius_;
}

glm::vec3 Enemy::getPosition() {
	return pos_;
}
