#include <missile.hpp>

void Missile::init() { //привязываем контекст
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_), vertex_, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Missile::freeResources() {
	glDeleteVertexArrays(1, &VAO_);
	glDeleteBuffers(1, &VBO_);
}

////////////////////////
Missile::Missile(const Camera& camera) : camera_(camera), start_pos_(camera.GetCameraPos()),
front_(camera.GetCameraFront()), up_(camera.GetCameraUp()),
cur_pos_(start_pos_), is_usable_(true), birth_time_(glfwGetTime()) {
	std::cout << "SHOOT!\n";
}

void Missile::drawMissile(GLuint programID, GLuint MatrixID) const {
	glm::mat4 model_ = glm::mat4(1.0f);
	model_ = glm::translate(model_, cur_pos_);
	glm::mat4 MVP_ = camera_.GetProjectionViewMatrix() * model_;
	glUseProgram(programID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_[0][0]);
	glBindVertexArray(VAO_);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void Missile::updatePosition() {
	if (getLifetime() > max_lifetime_) {
		is_usable_ = false;
		return;
	}
	cur_pos_ = start_pos_ + move_speed_ * getLifetime() * front_;
}

bool Missile::isExist() const {
	return is_usable_;
}

void Missile::markUnusable() {
	is_usable_ = false;
}

glm::vec3 Missile::getPosition() const{
	return cur_pos_;
}

GLfloat Missile::getLifetime() {
	return glfwGetTime() - birth_time_;
}