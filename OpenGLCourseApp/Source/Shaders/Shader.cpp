#include "Shader.h"

Shader::Shader() : ShaderID(0), uniformModel(0), uniformProjection(0), PointLightCount(0)
{

}

Shader::~Shader()
{	
	ClearShader();
}

void Shader::CreateFromString(const char* VertexCode, const char* FragmentCode)
{
	CompileShader(VertexCode, FragmentCode);
}

void Shader::CreateFromFiles(const char* VertexFilePath, const char* FragmentFilePath)
{
	std::string VertexString = ReadFile(VertexFilePath);
	std::string FragmentString = ReadFile(FragmentFilePath);
	const char* VertexCode = VertexString.c_str();
	const char* FragmentCode = FragmentString.c_str();

	CompileShader(VertexCode, FragmentCode);
}

std::string Shader::ReadFile(const char* FilePath)
{
	std::string content;
	std::ifstream FileStream(FilePath, std::iostream::in);
	if (!FileStream.is_open()) {
		std::cerr << "Error Reading file.";
		return std::string(); 
	}

	std::string line = "";
	while (!FileStream.eof())
	{
		std::getline(FileStream, line);
		content.append(line + "\n");
	}
	FileStream.close();
	return content;
}

GLuint Shader::GetProjectionLocation() const
{
	return uniformProjection;
}
GLuint Shader::GetModelLocation() const
{
	return uniformModel;
}
GLuint Shader::GetViewLocation() const
{
	return uniformView;
}
GLuint Shader::GetAmbientColourLocation() const
{
	return uniformDirectionalLight.uniformAmbientColour;
}
GLuint Shader::GetAmbientIntensityLocation() const
{
	return uniformDirectionalLight.uniformAmbientIntensity;
}
GLuint Shader::GetDiffuseIntensityLocation() const
{
	return uniformDirectionalLight.uniformDiffuseIntensity;
}
GLuint Shader::GetDirectionLocation()
{
	return uniformDirectionalLight.uniformDirection;
}
GLuint Shader::GetSpecularIntesityLocation() const
{
	return uniformSpecularIntensity;
}
GLuint Shader::GetShininessLocation()
{
	return uniformShininess;
}
GLuint Shader::GetEyePositionLocation()
{
	return uniformEyePosition;
}

// transfer locations of uniform variables to Light class
void Shader::SetDirectionalLight(DirectionalLight* DirLight)
{
	DirLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformAmbientColour,
		uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* PointLight, unsigned int LightCount)
{
	if (LightCount > MAX_POINT_LIGHTS) LightCount = MAX_POINT_LIGHTS;
	// Set PointLightCount to the uniform variable in the fragment shader
	glUniform1i(uniformPointLightCount, LightCount);

	for (size_t i = 0; i < LightCount; i++)
	{
		PointLight[i].UseLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformAmbientColour,
			uniformPointLights[i].uniformDiffuseIntensity, uniformPointLights[i].uniformPosition,
			uniformPointLights[i].uniformConstant, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformExponent);
	}
}

void Shader::SetSpotLights(SpotLight* SpotLight, unsigned int LightCount)
{
	if (LightCount > MAX_POINT_LIGHTS) LightCount = MAX_POINT_LIGHTS;
	// Set PointLightCount to the uniform variable in the fragment shader
	glUniform1i(uniformSpotLightCount, LightCount);

	for (size_t i = 0; i < LightCount; i++)
	{
		SpotLight[i].UseLight(uniformSpotLights[i].uniformAmbientIntensity, uniformSpotLights[i].uniformAmbientColour,
			uniformSpotLights[i].uniformDiffuseIntensity, uniformSpotLights[i].uniformPosition, uniformSpotLights[i].uniformDirection,
			uniformSpotLights[i].uniformConstant, uniformSpotLights[i].uniformLinear, uniformSpotLights[i].uniformExponent,
			uniformSpotLights[i].uniformEdge);
	}
}

void Shader::SetTexutre(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
	glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4* lTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}


void Shader::UseShader()
{
	glUseProgram(ShaderID);
}


void Shader::ClearShader()
{
	if (ShaderID != 0)
	{
		glDeleteProgram(ShaderID);
	}
	uniformModel = uniformProjection = 0;
}

// Compile our shader-program and get locations of uniform variables
void Shader::CompileShader(const char* VertexCode, const char* FragmentCode)
{
	ShaderID = glCreateProgram();

	if (!ShaderID) {
		printf("Error creating ShaderID program!\n");
		return;
	}
	AddShader(ShaderID, VertexCode, GL_VERTEX_SHADER);
	AddShader(ShaderID, FragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(ShaderID);
	//Check if program link probably
	glGetProgramiv(ShaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ShaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(ShaderID);
	glGetProgramiv(ShaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ShaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	//get id of our uniform variable
	uniformModel = glGetUniformLocation(ShaderID, "model");
	uniformProjection = glGetUniformLocation(ShaderID, "projection");
	uniformView = glGetUniformLocation(ShaderID, "view");
	// Directional light
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(ShaderID, "DirectionalLight.Base.AmbientIntensity");
	uniformDirectionalLight.uniformAmbientColour = glGetUniformLocation(ShaderID, "DirectionalLight.Base.Colour");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(ShaderID, "DirectionalLight.Base.DiffuseIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(ShaderID, "DirectionalLight.Direction");
	uniformEyePosition = glGetUniformLocation(ShaderID, "EyePosition");

	uniformShininess = glGetUniformLocation(ShaderID, "Material.Shininess");
	uniformSpecularIntensity = glGetUniformLocation(ShaderID, "Material.SpecularIntensity");

	uniformDirectionalLightTransform = glGetUniformLocation(ShaderID, "directionalLightTransform");
	uniformTexture = glGetUniformLocation(ShaderID, "TheTexture");
	uniformDirectionalShadowMap = glGetUniformLocation(ShaderID, "directionalShadowMap");

	// Point light setup
	uniformPointLightCount = glGetUniformLocation(ShaderID, "PointLightCount");
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Base.Colour", i);
		uniformPointLights[i].uniformAmbientColour = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Base.AmbientIntensity", i);
		uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Base.DiffuseIntensity", i);
		uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Position", i);
		uniformPointLights[i].uniformPosition = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Constant", i);
		uniformPointLights[i].uniformConstant = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Linear", i);
		uniformPointLights[i].uniformLinear = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "PointLights[%d].Exponent", i);
		uniformPointLights[i].uniformExponent = glGetUniformLocation(ShaderID, LocBuffer);
	}


	// Point light setup
	uniformSpotLightCount = glGetUniformLocation(ShaderID, "SpotLightCount");
	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Base.Colour", i);
		uniformSpotLights[i].uniformAmbientColour = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Base.AmbientIntensity", i);
		uniformSpotLights[i].uniformAmbientIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Base.DiffuseIntensity", i);
		uniformSpotLights[i].uniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Position", i);
		uniformSpotLights[i].uniformPosition = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Constant", i);
		uniformSpotLights[i].uniformConstant = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Linear", i);
		uniformSpotLights[i].uniformLinear = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Base.Exponent", i);
		uniformSpotLights[i].uniformExponent = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Direction", i);
		uniformSpotLights[i].uniformDirection = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "SpotLights[%d].Edge", i);
		uniformSpotLights[i].uniformEdge = glGetUniformLocation(ShaderID, LocBuffer);
	}
}

// Creates our shaders
void Shader::AddShader(GLuint ShaderProgramID, const char* ShaderCode, GLenum ShaderType)
{
	//Our shader
	GLuint theShader = glCreateShader(ShaderType);
	const GLchar* theCode[1]{};

	theCode[0] = ShaderCode;
	GLint codeLength[1]{};
	codeLength[0] = std::strlen(ShaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	//Check if program COMPILE probably
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", ShaderType, eLog);
		return;
	}
	glAttachShader(ShaderProgramID, theShader);
}


