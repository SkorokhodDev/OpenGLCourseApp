#include "PointLight.h"

PointLight::PointLight() : Light()
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Constant = 1.0f; // cant divide by zero
	Linear = 0.0f;
	Exponent = 0.0f;
}

PointLight::PointLight(GLfloat red, GLfloat green, GLfloat blue, 
						GLfloat AmIntensity, GLfloat DiffuseIntensity, 
						GLfloat xPos, GLfloat yPos, GLfloat zPos, 
						GLfloat con, GLfloat lin, GLfloat exp) : Light(red, green, blue, AmIntensity, DiffuseIntensity)
{
	Position = glm::vec3(xPos, yPos, zPos);
	Constant = con;
	Linear = lin;
	Exponent = exp;
}

void PointLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
	GLuint DiffuseIntensityLocation, GLuint PositionLocation,
	GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation)
{
	// Set values to the shader script
	glUniform3f(AmbientColourLocation, Colour.x, Colour.y, Colour.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);

	glUniform3f(PositionLocation, Position.x, Position.y, Position.z);
	glUniform1f(ConstantLocation, Constant);
	glUniform1f(LinearLocation, Linear);
	glUniform1f(ExponentLocation, Exponent);
}

PointLight::~PointLight()
{
}
