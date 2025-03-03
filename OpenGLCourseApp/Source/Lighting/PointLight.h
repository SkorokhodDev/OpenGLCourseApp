#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity, GLfloat DiffuseIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
		GLuint DiffuseIntensityLocation, GLuint PositionLocation,
		GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation);
	
	~PointLight();



protected:
	glm::vec3 Position;

	GLfloat Constant, Linear, Exponent; // L / (ax^2 + bx + c)
};

