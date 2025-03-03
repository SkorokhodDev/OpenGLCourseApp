#include "DirectionalLight.h"


DirectionalLight::DirectionalLight() : Light()
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat DiffuseIntensity) : Light(red, green, blue, AmIntensity, DiffuseIntensity)
{
	Direction = glm::vec3(xDir, yDir, zDir);
}


void DirectionalLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
	GLuint DiffuseIntensityLocation, GLuint DirectionLocation)
{
	// Set values to the shader script
	glUniform3f(AmbientColourLocation, Colour.x, Colour.y, Colour.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);

	glUniform3f(DirectionLocation, Direction.x, Direction.y, Direction.z);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
}
DirectionalLight::~DirectionalLight()
{
}
