#version 330

layout (location = 0) in vec3 pos;

uniform mat4 u_Model;
// (projection * view) how we convert this point into the point of view of our drirectional light
uniform mat4 u_DirectionalLightTransform;	

void main()
{
	gl_Position = u_DirectionalLightTransform * u_Model * vec4(pos, 1.0);
}