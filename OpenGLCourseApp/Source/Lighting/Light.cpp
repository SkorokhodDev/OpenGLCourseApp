#include "Light.h"

Light::Light()
{
	// How much of the colour should be shown
	Colour = glm::vec3(1.0f, 1.0f, 1.0f);
	AmbientIntensity = 1.0f;

	DiffuseIntensity = 0.0f;    
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat AmIntensity, GLfloat DiffuseIntensity)
{
	Colour = glm::vec3(red, green, blue);

	AmbientIntensity = AmIntensity;
	
	this->DiffuseIntensity = DiffuseIntensity;
}


//void Light::UseLight(GLfloat AmbientIntensityLocation, GLfloat AmbientColourLocation, GLfloat DiffuseIntensityLocation)
//{
//	// Set values to the shader script
//	glUniform3f(AmbientColourLocation, Colour.x, Colour.y, Colour.z);
//	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
//	
//	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
//}

Light::~Light()
{
}
