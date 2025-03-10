#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight();

	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat AmIntensity, GLfloat DiffuseIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat shadowWith, GLfloat shadowHeight);

	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
		GLuint DiffuseDirectionLocation, GLuint DirectionLocation);

	glm::mat4 CalculateDirLightTransform();

private:
	glm::vec3 Direction;
};

