#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aUV;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normal;

layout(set = 0, binding = 0) uniform CameraUBO {
	mat4 view;
	mat4 proj;
} cam;

layout(set = 2, binding = 0) uniform NodeUBO {
	mat4 model;
} node;


void main() {

	mat3 rotation = mat3(node.model);

    uv = aUV.xy;
    normal = rotation * aNormal;

	// output the position of each vertex
	gl_Position = cam.proj * cam.view * node.model * vec4(aPos, 1.0f);
	//gl_Position = cam.proj * cam.view * vec4(aPos, 1.0f);
}
