#version 450

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 uv;

layout(set = 1, binding = 0) uniform sampler2D diffuse;

void main() {
	
	out_color = texture(diffuse, uv);
}
