#version 330

in vec4 FragPos;

uniform vec3 OmniLightPos;
uniform float FarPlane;

void main()
{
	float Distance = length(FragPos.xyz - OmniLightPos);
	Distance = Distance/FarPlane; // normalize between 0 and 1
	gl_FragDepth = Distance;
}