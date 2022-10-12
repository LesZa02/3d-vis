#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

int main(void)
{
	//камера будет двигаться по окружности радиуса 15, со угловой скоростью Pi / 1000
	const double speed = M_PI / 5000;
	const double radius = 15;

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

	// Включить тест глубины
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//альфа-канальность
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//конец настройки



	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programIDtriangle1 = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint MatrixIDtriangle1 = glGetUniformLocation(programIDtriangle1, "MVP");
	GLuint programIDtriangle2 = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint MatrixIDtriangle2 = glGetUniformLocation(programIDtriangle2, "MVP");
	GLuint programIDaxis = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint MatrixIDaxis = glGetUniformLocation(programIDtriangle2, "MVP");


	int start = clock();
	double angle = (clock() - start) * speed;

	auto x_calc = [](double angle, int radius) { return radius * std::cos(angle); };
	auto y_calc = [](double angle, int radius) { return 7 * std::cos(2 * angle); };
	auto z_calc = [](double angle, int radius) { return radius * std::sin(angle); };

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(x_calc(angle, radius), y_calc(angle, radius), z_calc(angle, radius)),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0) 
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model; 

	//опеределяем первый треугольник
	static const GLfloat triangle1_vertex[] = {
		 2.0f, -2.5f, -6.0f,
		 -4.0f, -2.5f, 5.0f,
		 3.0f,  0.5f, -3.0f,
	}; 
	static const GLfloat triangle1_color[] = {
		1.0f,  0.0f,  0.0f, 0.2f,
		1.0f,  0.0f,  0.0f, 0.2f,
		1.0f,  0.0f,  0.0f, 0.2f,
	};
	//определяем второй треугольник
	static const GLfloat triangle2_vertex[] = {
		 -4.0f, -2.5f, -4.0f, 
		 4.0f, -2.5f, 2.0f,
		 -3.0f,  3.5f, -3.0f,
	}; 
	static const GLfloat triangle2_color[] = {
		0.0f,  1.0f,  0.0f, 0.2f,
		0.0f,  1.0f,  0.0f, 0.2f,
		0.0f,  1.0f,  0.0f, 0.2f,
	};
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

	GLuint triangle1_vertex_buf;
	glGenBuffers(1, &triangle1_vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, triangle1_vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1_vertex), triangle1_vertex, GL_STATIC_DRAW);

	GLuint triangle1_color_buf;
	glGenBuffers(1, &triangle1_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, triangle1_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1_color), triangle1_color, GL_STATIC_DRAW);

	GLuint triangle2_vertex_buf;
	glGenBuffers(1, &triangle2_vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, triangle2_vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2_vertex), triangle2_vertex, GL_STATIC_DRAW);

	GLuint triangle2_color_buf;
	glGenBuffers(1, &triangle2_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, triangle2_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2_color), triangle2_color, GL_STATIC_DRAW);


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
		//std::cout << angle << "\n";
		View = glm::lookAt(
			glm::vec3(x_calc(angle, radius), y_calc(angle, radius), z_calc(angle, radius)),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		MVP = Projection * View * Model;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //не забываем очищать z-буфер
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);


		//triangel1
		glUseProgram(programIDtriangle1);
		glUniformMatrix4fv(MatrixIDtriangle1, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, triangle1_vertex_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, triangle1_color_buf);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//triangle2
		glUseProgram(programIDtriangle2);
		glUniformMatrix4fv(MatrixIDtriangle2, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, triangle2_vertex_buf); //координаты
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, triangle2_color_buf); //цвет
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		//glDisable(GL_BLEND); 

		//axis
		glUseProgram(programIDaxis);
		glUniformMatrix4fv(MatrixIDaxis, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_buf); //координаты
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, axis_color_buf); //цвет
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_LINES, 0, 3 * 2);
		//end axis

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1); 

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &triangle1_vertex_buf);
	glDeleteBuffers(1, &triangle1_color_buf);
	glDeleteBuffers(1, &triangle2_vertex_buf);
	glDeleteBuffers(1, &triangle2_color_buf);
	glDeleteProgram(programIDtriangle1);
	glDeleteProgram(programIDtriangle2);
	glDeleteVertexArrays(1, &VertexArrayID);
	glfwTerminate();

	return 0;
}

