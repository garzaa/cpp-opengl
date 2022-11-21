#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

// need to send it to the fragment shader
out vec3 color;

// a uniform is something you can access anywhere (?)
// never declare uniforms if you don't use them, because they'll be deleted automatically by OpenGL
// and that will cause errors
uniform float scale;

void main() {
	gl_Position = vec4(aPos.x + aPos.x*scale, aPos.y + aPos.y*scale, aPos.z + aPos.z*scale, 1.0);
	color = aCol;
}