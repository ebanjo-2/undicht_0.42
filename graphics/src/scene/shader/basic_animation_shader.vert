#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 color;

layout(set = 0, binding = 0) uniform CameraUBO {
	mat4 view;
	mat4 proj;
} cam;

layout(set = 2, binding = 0) uniform NodeUBO {
	mat4 model;
} node;

const int MAX_BONES_PER_VERTEX = 4;

void main() {

	mat3 rotation = mat3(node.model);

    uv = aUV.xy;
    normal = rotation * aNormal;
    
    color = vec3(0.0, 0.0, 0.0);
    
    for(int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
    	if(aBoneIDs[i] == 0) color += vec3(1.0f, 0.0f, 0.0f) * aBoneWeights[i];
    	if(aBoneIDs[i] == 1) color += vec3(0.0f, 1.0f, 0.0f) * aBoneWeights[i];
    	if(aBoneIDs[i] == 2) color += vec3(0.0f, 0.0f, 1.0f) * aBoneWeights[i]; 
    }

	// output the position of each vertex
	gl_Position = cam.proj * cam.view * node.model * vec4(aPos, 1.0f);

}
