#include "DirectionalLight.h"


DirectionalLight::DirectionalLight() : Light()
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat DiffuseIntensity,
	GLfloat shadowWith, GLfloat shadowHeight) : Light(red, green, blue, AmIntensity, DiffuseIntensity, shadowWith, shadowHeight)
{
	Direction = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
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
glm::mat4 DirectionalLight::CalculateLightTransform()
{
	return lightProj * glm::lookAt(-Direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
DirectionalLight::~DirectionalLight()
{
}
