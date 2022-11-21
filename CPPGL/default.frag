#version 330 core
out vec4 FragColor;

// inputs and outputs are linked from in/out
in vec3 color;

void main() {
	FragColor = vec4(color, 1.0f);
}