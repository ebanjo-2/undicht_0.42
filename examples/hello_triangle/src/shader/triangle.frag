#version 450

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 vertex_color;

void main() {
	
	out_color = vec4(vertex_color, 0);
}
