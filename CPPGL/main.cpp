#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

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
	GLfloat vertices[] =
	{ //               COORDINATES                  /     COLORS           //
		-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
		 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
		 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
		-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
		 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
		 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
	};

	GLuint indices[] = {
		0, 3, 5, // lower left
		3, 2, 4, // lower right
		5, 4, 1 // top
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
	// the structure is [ x y z r g b | x y z r g b ]
	vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
	// initial offset for the colors (3 floats i.e. a float3 vector)
	vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3 * sizeof(float)));
	// unbind the rest to keep from modifyinig them and not having it be picked up?
	// or would modifying them just be slow
	vao1.Unbind();
	vbo1.Unbind();
	ebo1.Unbind();

	// to set a uniform, get its reference value in the main function
	// but you can't set it until after you activate the shader
	GLuint uniformScaleID = glGetUniformLocation(shaderProgram.ID, "scale");

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
		glUniform1f(uniformScaleID, 1.2f);
		// bind the vertex array to the current rendering cycle
		vao1.Bind();
		// primitive type, # of indices, data type of indices, start index (offset)
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
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

	// delete window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}