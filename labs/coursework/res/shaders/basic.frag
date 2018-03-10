#version 410

layout (location = 0) in vec4 vetex_colour;

layout (location = 0) out vec4 out_colour;

void main() {
	out_colour = vetex_colour;
}
