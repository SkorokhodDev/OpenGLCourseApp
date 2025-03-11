#version 330

in vec4 FragPos;

uniform vec3 u_OmniLightPos;
uniform float u_FarPlane;

void main()
{
	float Distance = length(FragPos.xyz - u_OmniLightPos);
	Distance = Distance/u_FarPlane; // normalize between 0 and 1
	gl_FragDepth = Distance;
}