#pragma once


#include<glad/glad.h>
#include"VBO.h"

class VAO {
public:
	GLuint ID;
	VAO();

	// link vertex buffer to vertex array
	void LinkVBO(VBO &vbo, GLuint layout);
	void Bind();
	void Unbind();
	void Delete();
};