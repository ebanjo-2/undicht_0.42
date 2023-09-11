#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aUV;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out  vec2 uv;

layout(set = 0, binding = 0) uniform CameraUBO {
	mat4 view;
	mat4 proj;
} cam;


void main() {

    uv = aUV.xy;

	//output the position of each vertex
	gl_Position = cam.proj * cam.view * vec4(aPos, 1.0f);

}
