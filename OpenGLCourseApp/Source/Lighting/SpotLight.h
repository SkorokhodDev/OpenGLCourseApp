#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight();
	SpotLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity, GLfloat DiffuseIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat con, GLfloat lin, GLfloat exp, GLfloat Edge,
		GLfloat shadowWidth, GLfloat shadowHeight, GLfloat nearPlane, GLfloat farPlane);

	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
		GLuint DiffuseIntensityLocation, GLuint PositionLocation, GLuint DirectionLocation,
		GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation, GLuint EdgeLocation);

	void SetFlash(glm::vec3 Position, glm::vec3 Direction);

	void Toggle() {};

	~SpotLight();

private:
	glm::vec3 Direction;

	GLfloat Edge, ProcessEdge;

};

