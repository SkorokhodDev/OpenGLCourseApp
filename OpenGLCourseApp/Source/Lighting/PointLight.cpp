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
						GLfloat con, GLfloat lin, GLfloat exp, 
						GLfloat shadowWidth, GLfloat shadowHeight, 
						GLfloat nearPlane, GLfloat farPlane ) : Light(red, green, blue, AmIntensity, DiffuseIntensity, shadowWidth, shadowHeight)
{
	Position = glm::vec3(xPos, yPos, zPos);
	Constant = con;
	Linear = lin;
	Exponent = exp;

	FarPlane = farPlane;

	float aspect = (float)shadowWidth / (float)shadowHeight;
	lightProj = glm::perspective(glm::radians(90.f), aspect, nearPlane, FarPlane);

	shadowMap = new OmniShadowMap();
	shadowMap->init(shadowWidth, shadowHeight);
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

std::vector<glm::mat4> PointLight::CalculateLightTransform()
{
	std::vector<glm::mat4> lightMatrices;
	// Should be the same order as GL_TEXTURE_CUBE_MAP_POSITIVE_X. Positive X
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0f, -1.0f, 0.0)));
	// Negative X
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0f, -1.0f, 0.0)));
	// Positive Y
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0f, 0.0f, 1.0)));
	// Negative Y
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0f, 0.0f, -1.0)));
	// Positive Z
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0f, -1.0f, 0.0)));
	// Negative Z
	lightMatrices.push_back(lightProj * glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0f, -1.0f, 0.0)));

	return lightMatrices;
}
