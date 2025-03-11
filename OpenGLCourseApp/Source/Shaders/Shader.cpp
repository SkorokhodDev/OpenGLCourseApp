


#include "Shader.h"

Shader::Shader() : ShaderID(0), uniformModel(0), uniformProjection(0), PointLightCount(0), SpotLightCount(0)
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

void Shader::CreateFromFiles(const char* VertexFilePath, const char* GeometryFilePath, const char* FragmentFilePath)
{
	std::string VertexString = ReadFile(VertexFilePath);
	std::string GeometryString = ReadFile(GeometryFilePath);
	std::string FragmentString = ReadFile(FragmentFilePath);
	const char* VertexCode = VertexString.c_str();
	const char* GeometryCode = GeometryString.c_str();
	const char* FragmentCode = FragmentString.c_str();

	CompileShader(VertexCode, GeometryCode, FragmentCode);
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
GLuint Shader::GetOmniLightPosLocation()
{
	return uniformOmniLightPos;
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

void Shader::SetPointLights(PointLight* PointLight, unsigned int LightCount, unsigned int initialTextureUnit, unsigned int offset)
{
	if (LightCount > MAX_POINT_LIGHTS) LightCount = MAX_POINT_LIGHTS;
	// Set PointLightCount to the uniform variable in the fragment shader
	glUniform1i(uniformPointLightCount, LightCount);

	for (size_t i = 0; i < LightCount; i++)
	{
		PointLight[i].UseLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformAmbientColour,
			uniformPointLights[i].uniformDiffuseIntensity, uniformPointLights[i].uniformPosition,
			uniformPointLights[i].uniformConstant, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformExponent);

		PointLight[i].GetShadowMap()->Read(GL_TEXTURE0 + initialTextureUnit + i);
		glUniform1i(uniformOmniShadowMaps[i + offset].uniformShadowMap, initialTextureUnit + i);
		glUniform1f(uniformOmniShadowMaps[i + offset].uniformFarPlane, PointLight[i].GetFarPlane());
	}
}

void Shader::SetSpotLights(SpotLight* SpotLight, unsigned int LightCount, unsigned int initialTextureUnit, unsigned int offset)
{
	if (LightCount > MAX_SPOT_LIGHTS) LightCount = MAX_SPOT_LIGHTS;
	// Set PointLightCount to the uniform variable in the fragment shader
	glUniform1i(uniformSpotLightCount, LightCount);

	for (size_t i = 0; i < LightCount; i++)
	{
		SpotLight[i].UseLight(uniformSpotLights[i].uniformAmbientIntensity, uniformSpotLights[i].uniformAmbientColour,
			uniformSpotLights[i].uniformDiffuseIntensity, uniformSpotLights[i].uniformPosition, uniformSpotLights[i].uniformDirection,
			uniformSpotLights[i].uniformConstant, uniformSpotLights[i].uniformLinear, uniformSpotLights[i].uniformExponent,
			uniformSpotLights[i].uniformEdge);

		SpotLight[i].GetShadowMap()->Read(GL_TEXTURE0 + initialTextureUnit + i);
		glUniform1i(uniformOmniShadowMaps[i + offset].uniformShadowMap, initialTextureUnit + i);
		glUniform1f(uniformOmniShadowMaps[i + offset].uniformFarPlane, SpotLight[i].GetFarPlane());
	}
}

void Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
	glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetOmniLightMatrices(std::vector<glm::mat4> inLightMetrices)
{
	for (size_t i = 0; i < 6; i++)
	{
		glUniformMatrix4fv(uniformLightMetrices[i], 1, GL_FALSE, glm::value_ptr(inLightMetrices[i]));
	}
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

	CompileProgram();
}

void Shader::CompileShader(const char* VertexCode, const char* GeometryCode, const char* FragmentCode)
{
	ShaderID = glCreateProgram();

	if (!ShaderID) {
		printf("Error creating ShaderID program!\n");
		return;
	}
	AddShader(ShaderID, VertexCode, GL_VERTEX_SHADER);
	AddShader(ShaderID, GeometryCode, GL_GEOMETRY_SHADER);
	AddShader(ShaderID, FragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
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

void Shader::Validate()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(ShaderID);
	glGetProgramiv(ShaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ShaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
}

void Shader::CompileProgram()
{
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

	//get id of our uniform variable
	uniformProjection = glGetUniformLocation(ShaderID, "u_Projection");
	uniformModel = glGetUniformLocation(ShaderID, "u_Model");
	uniformView = glGetUniformLocation(ShaderID, "u_View");
	// Directional light
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(ShaderID, "u_DirectionalLight.Base.AmbientIntensity");
	uniformDirectionalLight.uniformAmbientColour = glGetUniformLocation(ShaderID, "u_DirectionalLight.Base.Colour");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(ShaderID, "u_DirectionalLight.Base.DiffuseIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(ShaderID, "u_DirectionalLight.Direction");

	uniformEyePosition = glGetUniformLocation(ShaderID, "u_EyePosition");
	uniformShininess = glGetUniformLocation(ShaderID, "u_Material.Shininess");
	uniformSpecularIntensity = glGetUniformLocation(ShaderID, "u_Material.SpecularIntensity");

	// Point light setup
	uniformPointLightCount = glGetUniformLocation(ShaderID, "u_PointLightCount");
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Base.Colour", i);
		uniformPointLights[i].uniformAmbientColour = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Base.AmbientIntensity", i);
		uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Base.DiffuseIntensity", i);
		uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Position", i);
		uniformPointLights[i].uniformPosition = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Constant", i);
		uniformPointLights[i].uniformConstant = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Linear", i);
		uniformPointLights[i].uniformLinear = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_PointLights[%d].Exponent", i);
		uniformPointLights[i].uniformExponent = glGetUniformLocation(ShaderID, LocBuffer);
	}


	// Spot light setup
	uniformSpotLightCount = glGetUniformLocation(ShaderID, "u_SpotLightCount");
	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Base.Colour", i);
		uniformSpotLights[i].uniformAmbientColour = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Base.AmbientIntensity", i);
		uniformSpotLights[i].uniformAmbientIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Base.DiffuseIntensity", i);
		uniformSpotLights[i].uniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Position", i);
		uniformSpotLights[i].uniformPosition = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Constant", i);
		uniformSpotLights[i].uniformConstant = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Linear", i);
		uniformSpotLights[i].uniformLinear = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Base.Exponent", i);
		uniformSpotLights[i].uniformExponent = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Direction", i);
		uniformSpotLights[i].uniformDirection = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_SpotLights[%d].Edge", i);
		uniformSpotLights[i].uniformEdge = glGetUniformLocation(ShaderID, LocBuffer);
	}

	for (size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "u_OmniShadowMaps[%d].ShadowMap", i);
		uniformOmniShadowMaps[i].uniformShadowMap = glGetUniformLocation(ShaderID, LocBuffer);

		snprintf(LocBuffer, sizeof(LocBuffer), "u_OmniShadowMaps[%d].FarPlane", i);
		uniformOmniShadowMaps[i].uniformFarPlane = glGetUniformLocation(ShaderID, LocBuffer);
	}

	uniformDirectionalLightTransform = glGetUniformLocation(ShaderID, "u_DirectionalLightTransform");
	uniformTexture = glGetUniformLocation(ShaderID, "u_TheTexture");
	uniformDirectionalShadowMap = glGetUniformLocation(ShaderID, "u_DirectionalShadowMap");

	uniformOmniLightPos = glGetUniformLocation(ShaderID, "u_OmniLightPos");
	uniformFarPlane = glGetUniformLocation(ShaderID, "u_FarPlane");

	for (size_t i = 0; i < 6; i++)
	{
		char LocBuffer[100] = { '\0' };

		snprintf(LocBuffer, sizeof(LocBuffer), "u_LightMatrices[%d]", i);
		uniformLightMetrices[i] = glGetUniformLocation(ShaderID, LocBuffer);
	}

	return;
}


