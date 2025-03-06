#include "SpotLight.h"

SpotLight::SpotLight() : PointLight()
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	Edge = 0.0f;
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue,
	GLfloat AmIntensity, GLfloat DiffuseIntensity,
	GLfloat xPos, GLfloat yPos, GLfloat zPos,
	GLfloat xDir, GLfloat yDir, GLfloat zDir,
	GLfloat con, GLfloat lin, GLfloat exp, GLfloat Edge,
	GLfloat shadowWidth, GLfloat shadowHeight, GLfloat nearPlane, GLfloat farPlane)
	: PointLight(red, green, blue, 
		AmIntensity, DiffuseIntensity, 
		xPos, yPos, zPos, 
		con, lin, exp, 
		shadowWidth, shadowHeight, nearPlane, farPlane)
{
	Direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
	this->Edge = Edge;
	ProcessEdge = cosf(glm::radians(Edge));
}



void SpotLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColourLocation, 
	GLuint DiffuseIntensityLocation, GLuint PositionLocation, GLuint DirectionLocation, 
	GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation, GLuint EdgeLocation)
{
	PointLight::UseLight(AmbientIntensityLocation, AmbientColourLocation, DiffuseIntensityLocation, PositionLocation,
		ConstantLocation, LinearLocation, ExponentLocation);

	glUniform3f(DirectionLocation, Direction.x, Direction.y, Direction.z);
	glUniform1f(EdgeLocation, ProcessEdge);
}

void SpotLight::SetFlash(glm::vec3 Position, glm::vec3 Direction)
{
	this->Position = Position;
	this->Direction = Direction;

}

SpotLight::~SpotLight()
{
}
