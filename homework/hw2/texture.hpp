#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

#include <GLFW/glfw3.h>

GLuint loadBMP_custom(const char * imagepath);

GLuint loadDDS(const char * imagepath);


#endif