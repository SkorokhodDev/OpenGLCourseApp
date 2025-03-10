#version 330

layout (location = 0) in vec3 pos;

uniform mat4 model;
// (projection * view) how we convert this point into the point of view of our drirectional light
uniform mat4 directionalLightTransform;	

void main()
{
	gl_Position = directionalLightTransform * model * vec4(pos, 1.0);
}