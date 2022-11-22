#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "stb/stb_image.h"

const char *vertShaderSource =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main() {\n"
	"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0"
;

const char *fragShaderSource = 
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main() {\n"
		"FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
	"}\n\0"
;

int main() {
	glfwInit();

	// configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// initialize the window
	// last two are fullscreen and "not important"
	GLFWwindow *window = glfwCreateWindow(800, 800, "OUGH OUGH", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// bind it to the context
	glfwMakeContextCurrent(window);

	// load GLAD so it configures OpenGL
	gladLoadGL();

	// set up viewport, then buffers
	glViewport(0, 0, 800, 800);

	// set up vertices and etc
	// have to be between -1 and 1 for clip space (or is it device space here?)
	// or... they're right up against the frustum i guess, so it might just be normalized
	// Vertices coordinates
	GLfloat vertices[] =
	{ //     COORDINATES     /        COLORS      /   TexCoord  //
		-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
		-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
		 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
		 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0, 2, 1, // Upper triangle
		0, 3, 2 // Lower triangle
	};

	// all openGL things can only be accessed by reference

	Shader shaderProgram("default.vert", "default.frag");

	// create the vertex array object
	VAO vao1;
	vao1.Bind();

	// list of vertices
	VBO vbo1(vertices, sizeof(vertices));
	// list of elements, bind it to vertices
	EBO ebo1(indices, sizeof(indices));

	// link position, then color
	// the structure is [ x y z r g b u v | x y z r g b u v]
	vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0);
	// initial offset for the colors (3 floats i.e. a float3 vector)
	vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3 * sizeof(float)));
	// then texcoords
	vao1.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// unbind the rest to keep from modifyinig them and not having it be picked up?
	// or would modifying them just be slow
	vao1.Unbind();
	vbo1.Unbind();
	ebo1.Unbind();

	// to set a uniform, get its reference value in the main function
	// but you can't set it until after you activate the shader
	GLuint uniformScaleID = glGetUniformLocation(shaderProgram.ID, "scale");

	// TEXTURE STUFF
	int imgWidth, imgHeight, imgColChannels;
	// char pointer  and pass in the address of imgWidth, imgHeight not the things themselves
	unsigned char *bytes = stbi_load("pumpkin panic 2 1x.png", &imgWidth, &imgHeight, &imgColChannels, 0);
	
	GLuint texture;
	glGenTextures(1, &texture);
	// then assign the texture to a texture unit, which is a slot for a texture
	// they come together as a bundle of up to 16 (texcoord?)
	glActiveTexture(GL_TEXTURE0);
	// then after activating it, bind it with the texture reference value
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// ST = U(1-V)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// second to last is pixel data type
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint tex0uniform = glGetUniformLocation(shaderProgram.ID, "tex0)");
	shaderProgram.Activate();
	// this just unbinds it?
	glUniform1i(tex0uniform, 0);

	// handle closing events lol
	while (!glfwWindowShouldClose(window)) {
		// front color: displayed on screen
		// back buffer: written to in bg
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// clean back buffer and assign new color to it
		glClear(GL_COLOR_BUFFER_BIT);

		// here's the actual shape render code from the indices
		// say which shader program we want to use
		shaderProgram.Activate();
		// set the uniform now that it's active
		// name changes on datatype
		glUniform1f(uniformScaleID, 0.5f);
		// then also have to bind it in the current frame
		glBindTexture(GL_TEXTURE_2D, texture);
		// bind the vertex array to the current rendering cycle
		vao1.Bind();
		// primitive type, # of indices, data type of indices, start index (offset)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// clean the back buffer to paint it to the current screen
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// then clean up the previously created shaders
	// since they're already compiled and linked
	vao1.Delete();
	vbo1.Delete();
	ebo1.Delete();
	shaderProgram.Delete();
	glDeleteTextures(1, &texture);

	// delete window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}