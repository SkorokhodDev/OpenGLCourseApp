#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

class Light abstract
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity, GLfloat DiffuseIntensity);

	// Set values to uniform variables
	//void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation, GLuint DiffuseDirectionLocation);

	~Light();

protected:
	glm::vec3 Colour;
	GLfloat AmbientIntensity;

	GLfloat DiffuseIntensity;
};

