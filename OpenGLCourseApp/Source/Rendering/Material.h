#pragma once

#include <GL/Glew.h>

class Material
{
public:
	Material();
	Material(GLfloat specIntensity, GLfloat shine);
	~Material();

	void UseMaterial(GLuint SpecularIntensityLocation, GLuint ShininessLocation);

private:
	GLfloat SpecularIntensity;
	GLfloat Shininess;
};

