#pragma once


#include<glad/glad.h>
#include"VBO.h"

class VAO {
public:
	GLuint ID;
	VAO();

	// link vertex buffer to vertex array
	void LinkAttrib(
		VBO &vbo,
		GLuint layout,
		GLuint numComponents,
		GLenum type,
		GLsizeiptr stride,
		void *offset
	);
	void Bind();
	void Unbind();
	void Delete();
};