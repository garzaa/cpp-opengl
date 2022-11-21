#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

// need to send it to the fragment shader
out vec3 color;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	color = aCol;
}