#pragma once
#include <vector>
#include "Light.h"
#include "Shadows/OmniShadowMap.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity, GLfloat DiffuseIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat shadowWidth, GLfloat shadowHeight, GLfloat nearPlane, GLfloat farPlane);

	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation,
		GLuint DiffuseIntensityLocation, GLuint PositionLocation,
		GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation);
	
	~PointLight();

	inline GLfloat GetFarPlane() const { return FarPlane; }

	inline glm::vec3 GetPosition() const { return Position; }

	std::vector<glm::mat4>  CalculateLightTransform();

protected:
	glm::vec3 Position;

	GLfloat Constant, Linear, Exponent; // L / (ax^2 + bx + c)

	GLfloat FarPlane;
};

