#version 410

uniform mat4 MVP;
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_colour;

layout (location = 0) out vec4 vetex_colour;

void main() {
	gl_Position = MVP * vec4(position, 1.0);
	vetex_colour = in_colour;
}
