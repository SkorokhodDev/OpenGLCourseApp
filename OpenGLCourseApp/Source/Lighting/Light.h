#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shadows/ShadowMap.h"

class Light abstract
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat AmIntensity, GLfloat DiffuseIntensity, 
		GLfloat shadowWith, GLfloat shadowHeight);

	// Set values to uniform variables
	//void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation, GLuint DiffuseDirectionLocation);

	~Light();

	ShadowMap* GetShadowMap() { return shadowMap; }

protected:
	glm::vec3 Colour;
	GLfloat AmbientIntensity;

	GLfloat DiffuseIntensity;

	glm::mat4 lightProj;
	ShadowMap* shadowMap;
};

