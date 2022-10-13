#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

std::vector <std::vector<GLfloat>> color_calc(int rect_cnt, int face_cnt) {
	std::vector<std::vector<GLfloat>> color (rect_cnt * face_cnt, std::vector<GLfloat>(4));

	for (int f = 0; f < face_cnt; ++f) {
		for (int r = 0; r < rect_cnt; ++r) {
			color[f * rect_cnt + r] = { 1.0f / (1 + std::abs(r - rect_cnt / 2)) ,
				0.2f,
				1.0f - 1.0f / (1 + std::abs(r - rect_cnt / 2)),
				0.5f - 0.25f * (f % 8 >= 4)};
		}
	}
	return color;
}

int main(void)
{
	//настраиваем общие параметры и настройки
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Homework", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

	// ¬ключить тест глубины
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//альфа-канальность
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//конец настройки



	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programIDring = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint MatrixIDring = glGetUniformLocation(programIDring, "MVP");
	GLuint programIDaxis = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint MatrixIDaxis = glGetUniformLocation(programIDaxis, "MVP");


	/// натройски движени€ камеры
	//камера будет двигатьс€ по горизонтальной окружности радиуса 7, со угловой скоростью Pi / 1000 и 
	//лелать обороты в вертикальной плоскости по окружности радиуса 5.5
	const double speed = M_PI / 5000;
	const double radius = 7;

	int start = clock();
	double angle = (clock() - start) * speed;

	auto x_calc = [](double angle, int radius) { return (radius + (radius - 1.5) * std::cos(0.5 * angle)) * std::cos(angle); };
	auto y_calc = [](double angle, int radius) { return 0 + radius * std::sin(0.5 * angle); };
	auto z_calc = [](double angle, int radius) { return (radius + (radius - 1.5) * std::cos(0.5 * angle)) * std::sin(angle); };

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(x_calc(angle, radius), y_calc(angle, radius), z_calc(angle, radius)),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	//строим саму фигуру
	const int face_cnt = 4;
	const int rect_cnt = 4;
	const int p_in_rect = 6;
	double pangle = 2 * M_PI / rect_cnt;
	int inPRadius = 4;
	int outPRadius = 6;
	int lb = -1, ub = 1;

	const int sz = p_in_rect * rect_cnt * face_cnt;
	static GLfloat ring_vertex[3 * sz];
	//внутр€нн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		ring_vertex[3 * p_in_rect * i] = inPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * i + 1] = lb;
		ring_vertex[3 * p_in_rect * i + 2] = inPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * i + 3] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * i + 4] = lb;
		ring_vertex[3 * p_in_rect * i + 5] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * i + 6] = inPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * i + 7] = ub;
		ring_vertex[3 * p_in_rect * i + 8] = inPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * i + 9] = inPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * i + 10] = ub;
		ring_vertex[3 * p_in_rect * i + 11] = inPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * i + 12] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * i + 13] = lb;
		ring_vertex[3 * p_in_rect * i + 14] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * i + 15] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * i + 16] = ub;
		ring_vertex[3 * p_in_rect * i + 17] = inPRadius * std::cos((i + 1) * pangle);
	}
	//внешн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		ring_vertex[3 * p_in_rect * (rect_cnt + i)] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 1] = lb;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 2] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 3] = outPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 4] = lb;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 5] = outPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 6] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 7] = ub;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 8] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 9] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 10] = ub;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 11] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 12] = outPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 13] = lb;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 14] = outPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 15] = outPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 16] = ub;
		ring_vertex[3 * p_in_rect * (rect_cnt + i) + 17] = outPRadius * std::cos((i + 1) * pangle);
	}
	//верхн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i)] = inPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 1] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 2] = inPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 3] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 4] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 5] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 6] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 7] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 8] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 9] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 10] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 11] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 12] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 13] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 14] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 15] = outPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 16] = ub;
		ring_vertex[3 * p_in_rect * (2 * rect_cnt + i) + 17] = outPRadius * std::cos((i + 1) * pangle);
	}
	//нижн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i)] = inPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 1] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 2] = inPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 3] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 4] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 5] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 6] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 7] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 8] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 9] = outPRadius * std::sin(i * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 10] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 11] = outPRadius * std::cos(i * pangle);

		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 12] = inPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 13] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 14] = inPRadius * std::cos((i + 1) * pangle);

		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 15] = outPRadius * std::sin((i + 1) * pangle);
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 16] = lb;
		ring_vertex[3 * p_in_rect * (3 * rect_cnt + i) + 17] = outPRadius * std::cos((i + 1) * pangle);
	}

	//пронумеруем точки и вычислим их цвета: нумераци€ по кругу, 
	//сначала верхние, потом нижние, от внутренних к внешним
	std::vector<std::vector<GLfloat>> color = color_calc(rect_cnt, face_cnt);
	static GLfloat ring_color[4 * sz];
	//внутр€нн€€ область
	for (int i = 0; i < rect_cnt; i++) { //нижн€€ лева€ точка
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * i + p] = color[i + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) { //нижн€€ права€ точка
			ring_color[4 * p_in_rect * i + 4 + p] = color[(i + 1) % rect_cnt + 2 * rect_cnt][p];
			ring_color[4 * p_in_rect * i + 16 + p] = color[(i + 1) % rect_cnt + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) { //верхн€€ лева€ точка
			ring_color[4 * p_in_rect * i + 8 + p] = color[i][p];
			ring_color[4 * p_in_rect * i + 12 + p] = color[i][p];

		}
		for (int p = 0; p < 4; ++p) { //верхн€€ права€ точка
			ring_color[4 * p_in_rect * i + 20 + p] = color[(i + 1) % rect_cnt][p];
		}
	}
	//внешн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (rect_cnt + i) + p] = color[i + rect_cnt + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (rect_cnt + i) + 4 + p] = color[(i + 1) % rect_cnt + rect_cnt + 2 * rect_cnt][p];
			ring_color[4 * p_in_rect * (rect_cnt + i) + 16 + p] = color[(i + 1) % rect_cnt + rect_cnt + 2 * rect_cnt][p];
		}						  
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (rect_cnt + i) + 8 + p] = color[i + rect_cnt][p];
			ring_color[4 * p_in_rect * (rect_cnt + i) + 12 + p] = color[i + rect_cnt][0 + p];
		}					
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (rect_cnt + i) + 20 + p] = color[(i + 1) % rect_cnt + rect_cnt][p];
		}
	}
	//верхн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + p] = color[i][p];
		}			
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + 4 + p] = color[(i + 1) % rect_cnt][p];
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + 16 + p] = color[(i + 1) % rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + 8 + p] = color[i + rect_cnt][p];
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + 12 + p] = color[i + rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (2 * rect_cnt + i) + 20 + p] = color[(i + 1) % rect_cnt + rect_cnt][p];
		}
	}
	//нижн€€ область
	for (int i = 0; i < rect_cnt; i++) {
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + p] = color[i + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + 4 + p] = color[(i + 1) % rect_cnt + 2 * rect_cnt][p];
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + 16 + p] = color[(i + 1) % rect_cnt + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + 8 + p] = color[i + rect_cnt + 2 * rect_cnt][p];
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + 12 + p] = color[i + rect_cnt + 2 * rect_cnt][p];
		}
		for (int p = 0; p < 4; ++p) {
			ring_color[4 * p_in_rect * (3 * rect_cnt + i) + 20 +p] = color[(i + 1) % rect_cnt + rect_cnt + 2 * rect_cnt][p];
		}
	}

	static const GLfloat axis_vertex[] = {
		-10.0f, 0.0f, 0.0f, // ось X
		10.0f, 0.0f, 0.0f,
		0.0f, -10.0f, 0.0f, // ось Y
		0.0f, 10.0f, 0.0f,
		0.0f, 0.0f, -10.0f,
		0.0f, 0.0f, 10.0f,
	};

	static const GLfloat axis_color[] = {
		1.0f,  1.0f,  1.0f, 0.5f, // ось X
		1.0f,  1.0f,  1.0f, 0.5f,
		1.0f,  1.0f,  1.0f, 0.5f, // ось Y
		1.0f,  1.0f,  1.0f, 0.5f,
		1.0f,  1.0f,  1.0f, 0.5f, // ось Z
		1.0f,  1.0f,  1.0f, 0.5f,
	};

	GLuint ring_vertex_buf;
	glGenBuffers(1, &ring_vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, ring_vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ring_vertex), ring_vertex, GL_STATIC_DRAW);

	GLuint ring_color_buf;
	glGenBuffers(1, &ring_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, ring_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ring_color), ring_color, GL_STATIC_DRAW);

	GLuint axis_vertex_buf;
	glGenBuffers(1, &axis_vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertex), axis_vertex, GL_STATIC_DRAW);

	GLuint axis_color_buf;
	glGenBuffers(1, &axis_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, axis_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_color), axis_color, GL_STATIC_DRAW);

	do {
		angle = (clock() - start) * speed;
		View = glm::lookAt(
			glm::vec3(x_calc(angle, radius), y_calc(angle, radius), z_calc(angle, radius)),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		MVP = Projection * View * Model;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //не забываем очищать z-буфер
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);


		//ring
		glUseProgram(programIDring);
		glUniformMatrix4fv(MatrixIDring, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, ring_vertex_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, ring_color_buf);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sz);


		//axis
		glUseProgram(programIDaxis);
		glUniformMatrix4fv(MatrixIDaxis, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_buf); //координаты
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, axis_color_buf); //цвет
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_LINES, 0, sz);
		//end axis

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &ring_vertex_buf);
	glDeleteBuffers(1, &ring_color_buf);
	glDeleteProgram(programIDring);
	glDeleteProgram(programIDaxis);
	glDeleteVertexArrays(1, &VertexArrayID);
	glfwTerminate();

	return 0;
}

