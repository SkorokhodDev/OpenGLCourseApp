#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include "GL/glew.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"

#include "../Lighting/DirectionalLight.h"
#include "../Lighting/PointLight.h"
#include "../Lighting/SpotLight.h"

#include "../CommonValues.h"


class Shader
{
public:
	Shader();
	~Shader();

	void CreateFromString(const char* VertexCode, const char* FragmentCode);
	void CreateFromFiles(const char* VertexShFilePath, const char* FragmentShFilePath);
	
	std::string ReadFile(const char* FilePath);

	 GLuint GetProjectionLocation() const;
	 GLuint GetModelLocation() const;
	 GLuint GetViewLocation() const;
	
	 GLuint GetAmbientColourLocation() const;
	 GLuint GetAmbientIntensityLocation() const;
	 GLuint GetDiffuseIntensityLocation() const;
	 GLuint GetDirectionLocation();

	 GLuint GetSpecularIntesityLocation() const;
	 GLuint GetShininessLocation();

	 GLuint GetEyePositionLocation();

	// Set light functions
	void SetDirectionalLight(DirectionalLight* DirLight);
	void SetPointLights(PointLight* PointLight, unsigned int LightCount);
	void SetSpotLights(SpotLight* PointLight, unsigned int LightCount);
	
	void SetTexutre(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetDirectionalShadowMap(GLuint textureUnit);

	void UseShader();
	void ClearShader();


private:
	int PointLightCount;
	int SpotLightCount;

	///////// Locations of uniform variables
	GLuint ShaderID, uniformProjection, uniformModel, uniformView, // Based values
		uniformSpecularIntensity, uniformShininess, uniformEyePosition,  // Materials
		uniformTexture, uniformDirectionalLightTransform, uniformDirectionalShadowMap;

	struct { // Light values
		GLuint uniformAmbientColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight; // Already created instance of this struct. C++ tip


	GLuint uniformPointLightCount;

	struct { // Point light values
		GLuint uniformAmbientColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLights[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

	struct { // Point light values
		GLuint uniformAmbientColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLights[MAX_SPOT_LIGHTS];

	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType);
};

