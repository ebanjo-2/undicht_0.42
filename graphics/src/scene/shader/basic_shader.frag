#version 450

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normal;

layout(set = 1, binding = 0) uniform sampler2D diffuse;

void main() {

	vec3 sun_dir = normalize(vec3(1,1,1));
	float light = clamp(dot(sun_dir, normal), 0.1, 1.0);
	
	out_color = texture(diffuse, uv) * light;
	
}
