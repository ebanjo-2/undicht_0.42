#version 450

const int MAX_BONES_PER_VERTEX = 4;
const int MAX_BONES_PER_MESH = 100;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normal;

layout(set = 0, binding = 0) uniform CameraUBO {
	mat4 view;
	mat4 proj;
} cam;

layout(set = 2, binding = 0) uniform NodeUBO {
	mat4 model;
	mat4 bones[MAX_BONES_PER_MESH];
} node;


void main() {

	// combining the bone transformations
	//mat4 bone_to_bind_pose = node.bones[0];
	mat4 bone_to_bind_pose = mat4(0.0f);
	for(int i = 0; i < MAX_BONES_PER_VERTEX; i++)
		bone_to_bind_pose += node.bones[aBoneIDs[i]] * aBoneWeights[i];

	mat4 model = node.model * bone_to_bind_pose;

	// calculate the outputs to the fragment shader
    uv = aUV.xy;
	mat3 rotation = mat3(model);
    normal = rotation * aNormal;

	// output the position of each vertex
	gl_Position = cam.proj * cam.view * model * vec4(aPos, 1.0f);

}