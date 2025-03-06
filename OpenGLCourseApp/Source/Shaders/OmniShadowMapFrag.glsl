#version 330

in vec4 FragPos;

uniform vec3 LightPos;
uniform float FarPlane;

void main()
{
	float Distance = length(FragPos.xyz - LightPos);
	Distance = Distance/FarPlane; // normalize between 0 and 1
	gl_FragDepth = Distance;
}