#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

	// all openGL things can only be accessed by reference

	// create a shader...it's mapped to an int because it's just stored at a certain position somewhere?
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// tell the vertex shader to use one string, source code as the shader source, and NULL isn't important
	glShaderSource(vertexShader, 1, &vertShaderSource, NULL);
	glCompileShader(vertexShader);
	
	// and do the same for the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	// after creating the shader program, attach the shaders by reference (&) would dereference them into literal values
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// then clean up the old vertex/fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertices and etc
	// have to be between -1 and 1 for clip space (or is it device space here?)
	// or... they're right up against the frustum i guess, so it might just be normalized
	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // lower left
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // lower right
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // top
		-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // middle left
		0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // middle right
		0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // middle bottom
	};

	GLuint indices[] = {
		0, 3, 5, // lower left
		3, 2, 4, // lower right
		5, 4, 1 // top
	};

	// send vertex data to the GPU
	// uploading data from CPU -> GPU is slow, so use batches (called buffers)
	// first, create the buffer object (use a reference int)
	GLuint VAO, VBO, EBO;
	// we only have 1 for the object
	// THIS HAS TO COME FIRST
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// binding in openGL means that we make a certain object the Current Object
	// which is targeted by random functions
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// store vertices in VBP
	// first we set the array data, static_draw optimizes as read once to improve performance
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// bind the EBO and say it's an element array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// add the indices to the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// opengl doesn't know where to find it, make a new vertex array object (buffer geometry)
	// stores pointers to one or more VBOs and tells OpenGL how to interpret them
	// position of vertex attribute, how many values we have per vertex...sound familiar?, vertex value type, stride (value size), offset
	glVertexAttribPointer(0, 3, GL_FLOAT, 3 * sizeof(float), 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// again, order is important here
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// handle closing events lol
	while (!glfwWindowShouldClose(window)) {
		// front color: displayed on screen
		// back buffer: written to in bg
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// clean back buffer and assign new color to it
		glClear(GL_COLOR_BUFFER_BIT);

		// here's the actual shape render code from the indices
		// say which shader program we want to use
		glUseProgram(shaderProgram);
		// bund the vertex array
		glBindVertexArray(VAO);
		// primitive type, # of indices, data type of indices, start index (offset)
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		// clean the back buffer to paint it to the current screen
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// then clean up the previously created shaders
	// since they're already compiled and linked
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}