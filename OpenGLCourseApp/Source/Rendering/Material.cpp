#include "Material.h"

Material::Material()
{
	SpecularIntensity = 0;
	Shininess = 0;
}

Material::Material(GLfloat specIntensity, GLfloat shine)
{
	SpecularIntensity = specIntensity;
	Shininess = shine;
}

Material::~Material()
{
}

// Put the Material data to the uniform values
void Material::UseMaterial(GLuint SpecularIntensityLocation, GLuint ShininessLocation)
{
	glUniform1f(SpecularIntensityLocation, SpecularIntensity);
	glUniform1f(ShininessLocation, Shininess);
}
