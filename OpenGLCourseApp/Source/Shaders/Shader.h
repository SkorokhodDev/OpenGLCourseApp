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
	void CreateFromFiles(const char* VertexFilePath, const char* FragmentFilePath);
	void CreateFromFiles(const char* VertexFilePath, const char* GeometryFilePath, const char* FragmentFilePath);
	
	void Validate();

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
	 GLuint GetOmniLightPosLocation(); //const { return uniformOmniLightPos; };
	 inline GLuint GetFarPlaneLocation() const { return uniformFarPlane; };

	 GLuint GetEyePositionLocation();

	// Set light functions
	void SetDirectionalLight(DirectionalLight* DirLight);
	void SetPointLights(PointLight* PointLight, unsigned int LightCount, unsigned int initialTextureUnit, unsigned int offset);
	void SetSpotLights(SpotLight* PointLight, unsigned int LightCount, unsigned int initialTextureUnit, unsigned int offset);
	
	void SetTexture(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetOmniLightMatrices(std::vector<glm::mat4> inLightMetrices);

	void UseShader();
	void ClearShader();


private:
	int PointLightCount;
	int SpotLightCount;

	///////// Locations of uniform variables
	GLuint ShaderID, uniformProjection, uniformModel, uniformView, // Based values
		uniformSpecularIntensity, uniformShininess, uniformEyePosition,  // Materials
		uniformTexture, uniformDirectionalLightTransform, uniformDirectionalShadowMap,
		uniformOmniLightPos, uniformFarPlane;

	GLuint uniformLightMetrices[6];

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

	struct {
		GLuint uniformShadowMap;
		GLuint uniformFarPlane;
	} uniformOmniShadowMaps[MAX_SPOT_LIGHTS + MAX_POINT_LIGHTS];

	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void CompileShader(const char* VertexCode, const char* GeometryCode, const char* FragmentCode);
	void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType);

	void CompileProgram();
};

