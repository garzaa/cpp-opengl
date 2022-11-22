#version 330 core
out vec4 FragColor;

// inputs and outputs are linked from in/out
in vec3 color;
in vec2 texcoord;

// because this is tex0
uniform sampler2D tex0;

void main() {
	// ST and UV are upside down, so reverse it here
	FragColor = texture(tex0, vec2(texcoord.s, 1.0-texcoord.t));
}