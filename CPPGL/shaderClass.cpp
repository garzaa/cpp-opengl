#include "shaderClass.h"

std::string get_file_contents(const char *filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char *vertexFile, const char *fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char *vertexSource = vertexCode.c_str();
	const char *fragmentSource = fragmentCode.c_str();

	// create a shader...it's mapped to an int because it's just stored at a certain position somewhere?
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// tell the vertex shader to use one string, source code as the shader source, and NULL isn't important
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// and do the same for the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ID = glCreateProgram();
	// after creating the shader program, attach the shaders by reference (&) would dereference them into literal values
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	// then clean up the old vertex/fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}