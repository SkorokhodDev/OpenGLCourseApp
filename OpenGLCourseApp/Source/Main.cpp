#define _USE_MATH_DEFINES

#include <stdio.h>
#include <cstring> //std::strlen
#include <cmath>
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Rendering/Mesh.h"
#include "Shaders/Shader.h"
#include "Window/Window.h"
#include "Window/Camera.h"

#include "Rendering/Texture.h"
#include "Rendering/Material.h"

#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"

#include "Rendering/Model.h"

#include "CommonValues.h"

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		//abort();
	}
}

#define GL_CHECK(stmt) do { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

std::vector<Mesh*> MeshList; 
std::vector<Shader> ShaderList; 
Shader directionalShadowShader;
Shader omniShadowShader;

//Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 2.0f, 0.2f);
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

DirectionalLight MainLight;
PointLight PointLights[MAX_POINT_LIGHTS];
SpotLight SpotLights[MAX_SPOT_LIGHTS];
unsigned int PointLightCount = 0;
unsigned int SpotLightCount = 0;

// Locations of our uniforms
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
GLuint uniformSpecularIntensity = 0, uniformShininess = 0, uniformEyePosition = 0;
GLuint uniformOmniLightPos = 0, uniformFarPlane = 0;
GLuint uniformDirectionalLightTransform = 0; // TODO: FIX THIS, no use

// Window Dimensions
constexpr float toRadians = static_cast<float>(M_PI / 180.0f);

// Vertex Shader
static const char* vShader = "Source/Shaders/VertexShader.glsl";
//Fragment Shader
static const char* fShader = "Source/Shaders/FragmentShader.glsl";


void CalcAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat* vertices, unsigned int verticesCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indicesCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		// substracting 3 vertex with first
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		// substracting second vertex with first
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2); // effect "/\" vectors

		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	// Writing normolized values to vertices (in nx,ny,nz);
	for (size_t i = 0; i < verticesCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset; // Start of the normal offset
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	GLfloat vertices[] = {
	//	x		 y		 z			u	   v		nx	  ny	nz
		-1.0f,  -1.0f,  -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f,   -1.0f,   1.0f, 		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f,   -1.0f,  -0.6f, 		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f,   1.0f,    0.0f,   	0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};


	GLfloat floorVertices[] = {
		//	x	 y		z		u	  v			nx	  ny	nz
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	CalcAverageNormals(indices, sizeof(indices) / sizeof(indices[0]), 
		vertices, sizeof(vertices) / sizeof(vertices[0]), 
		8, 5);

	// Creating Mesh 1
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, sizeof(vertices) / sizeof(vertices[0]), sizeof(indices) / sizeof(indices[0]));
	MeshList.push_back(obj1);

	Mesh* Floor = new Mesh();
	Floor->CreateMesh(floorVertices, floorIndices, 
		sizeof(floorVertices) / sizeof(floorVertices[0]), sizeof(floorIndices) / sizeof(floorIndices[0]));
	MeshList.push_back(Floor);

}

void CreateShaders()
{
	// Create shader1
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	ShaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Source/Shaders/DirShadowMapVert.glsl", "Source/Shaders/DirShadowMapFrag.glsl");

	omniShadowShader = Shader();
	omniShadowShader.CreateFromFiles("Source/Shaders/OmniShadowMapVert.glsl", "Source/Shaders/OmniShadowMapGeom.glsl", "Source/Shaders/OmniShadowMapFrag.glsl");
}


///////////////////// Load Texture 
Texture Tex_Brick;
Texture Tex_Dirt;
Texture Tex_Plain;

//////////////////// Materials
Material ShinyMaterial;
Material DullMaterial;

////////////////////// Models
Model Xwing;

Model Helicopter;
GLfloat HelicopterAngle = 0.0f;

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	model = glm::rotate(model, 15 * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	// attach our projection matrix to shader uniform variable
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Tex_Brick.UseTexture();
	ShinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess); // set Material class data to the uniformValues
	MeshList[0]->RenderMesh(); // render, where we actually calling shaders

	/////////////// FLooor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Tex_Dirt.UseTexture();
	ShinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	MeshList[1]->RenderMesh();

	//////////////////// Xwing
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 15.0f));
	model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//Tex_Dirt.UseTexture();
	ShinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Xwing.RenderModel();
	
	/////////////////// Helicopter
	HelicopterAngle += 0.1f;
	if (HelicopterAngle > 360.f)
	{
		HelicopterAngle = 0.1f;
	}
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -HelicopterAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
	model = glm::rotate(model, -20.f * toRadians, glm::vec3(0.0,0.0,1.0f));	// rotate around z
	model = glm::rotate(model, -90.f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1, 1, 1)); // ~0.4
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//Tex_Dirt.UseTexture();
	ShinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Helicopter.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light) //upgrade to several lights if i have more then one dir light
{
	directionalShadowShader.UseShader();

	// frame buffer is the same size as viewport
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	glm::mat4 lightTransform = light->CalculateDirLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	directionalShadowShader.Validate();


	GLuint testDebug = glGetUniformLocation(directionalShadowShader.GetShaderID(), "model_test");
	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* pLight)
{
	omniShadowShader.UseShader();

	// frame buffer is the same size as viewport
	glViewport(0, 0, pLight->GetShadowMap()->GetShadowWidth(), pLight->GetShadowMap()->GetShadowHeight());

	pLight->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();


	glUniform3f(uniformOmniLightPos, pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z);
	glUniform1f(uniformFarPlane, pLight->GetFarPlane());
	omniShadowShader.SetOmniLightMatrices(pLight->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	// our rendering
	ShaderList[0].UseShader();
	uniformModel = ShaderList[0].GetModelLocation();
	uniformProjection = ShaderList[0].GetProjectionLocation();
	uniformView = ShaderList[0].GetViewLocation();
	uniformEyePosition = ShaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = ShaderList[0].GetSpecularIntesityLocation();
	uniformShininess = ShaderList[0].GetShininessLocation();

	glViewport(0, 0, 1366, 768);

	// Clear window 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);

	ShaderList[0].SetDirectionalLight(&MainLight);
	ShaderList[0].SetPointLights(PointLights, PointLightCount, 3, 0);
	ShaderList[0].SetSpotLights(SpotLights, SpotLightCount, 3 + PointLightCount, PointLightCount); // set offset
	glm::mat4 lightTransform = MainLight.CalculateDirLightTransform();
	ShaderList[0].SetDirectionalLightTransform(&lightTransform);


	MainLight.GetShadowMap()->Read(GL_TEXTURE2);

	ShaderList[0].SetTexture(1);
	ShaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 LowerLight = camera.GetCameraPosition();
	LowerLight.y -= 0.03f; // -=0.3f
	SpotLights[0].SetFlash(LowerLight, camera.GetCameraDirection());

	ShaderList[0].Validate();

	RenderScene();
}


int main()
{
	Window* MainWindow = new Window(1366, 768);
	MainWindow->Initialize();

	CreateObjects();
	CreateShaders();

	///////////////////// Load Texture 
	Tex_Brick = Texture("Resources/Textures/brick.png");
	Tex_Brick.LoadTexture(true);
	Tex_Dirt = Texture("Resources/Textures/dirt.png");
	Tex_Dirt.LoadTexture(true);
	Tex_Plain = Texture("Resources/Textures/plain.png");
	Tex_Plain.LoadTexture(true);

	//////////////////// Materials
	ShinyMaterial = Material(4.0f, 256);
	DullMaterial = Material(0.3f, 4);

	////////////////////// Models
	Xwing = Model();
	Xwing.LoadModel("Resources/Models/x-wing.obj");

	Helicopter = Model();
	Helicopter.LoadModel("Resources/Models/cube.obj");


	///////////////////// L I G H T I N G
	//Initialize DirectionalLight
	MainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.1f, 0.2f, // 0,3 0.6
								0.0f, -15.0f, -10.0f, 
								2048, 2048);

	// Initialize PointLights
	PointLights[0] = PointLight(0.0f, 0.0f, 1.0f, // colour
								0.2f, 1.0f,
								1.0f, 2.0f, 0.0f, // pos
								0.3f, 0.01f, 0.01f,
								1024, 1024, 0.01f, 100.0f);
	PointLightCount++;
	PointLights[1] = PointLight(0.0f, 1.0f, 0.0f, // colour
								0.2f, 1.0f,
								-4.0f, 2.0f, 0.0f, // pos
								0.3f, 0.01f, 0.01f,
								1024, 1024, 0.01f, 100.0f);
	PointLightCount++;
	
	// Initialize SpotLights
	SpotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,    // Flash light
								0.0f, 2.0f,
								0.0f, 0.0f, 0.0f, //pos
								0.0f, -1.0f, 0.0f, // dir
								1.0f, 0.0f, 0.0f, 20.0f, // edge
								1024, 1024, 0.01f, 100.0f);
	SpotLightCount++;
	SpotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
								1.0f, 3.0f,
								3.0f, 0.0f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f, 20.0f,
								1024, 1024, 0.01f, 100.0f);
	//SpotLightCount++;



	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)MainWindow->GetBufferWidth() / MainWindow->GetBufferHeight(), 0.1f, 100.0f);

	GLfloat DeltaTime = 0.0f;
	GLfloat LastTime = 0.0f;

	// Main loop
	while (!MainWindow->GetShouldClose())
	{

		GLfloat CurrentTime = glfwGetTime(); // STD_GetPerfomanceCounter();
		DeltaTime = CurrentTime - LastTime; // (CurrentTimme - LastTime) * 1000 / STD_GetPerfomanceFrequency(); // 1000* to convert into seconds
		LastTime = CurrentTime;

		// Get + Handle user input events (resize and etc)
		glfwPollEvents();

		// Handle camera input
		camera.KeyControl(MainWindow->GetKeys(), DeltaTime);
		camera.MouseControl(MainWindow->GetXChange(), MainWindow->GetYChange());

		if (MainWindow->GetKeys()[GLFW_KEY_L])
		{
			SpotLights[0].Toggle();
			MainWindow->GetKeys()[GLFW_KEY_L] = false;
		}

		// This will render the screnn, but not to the viewport.
		// It will render it to a frame buffer, which will then save it to a texture.
		// Then this texture will be used in next pass;
		DirectionalShadowMapPass(&MainLight);
		for (size_t i = 0; i < PointLightCount; i++)
		{
			OmniShadowMapPass(&PointLights[i]);
		}
		for (size_t i = 0; i < SpotLightCount; i++)
		{
			OmniShadowMapPass(&SpotLights[i]);
		}

		RenderPass(projection, camera.CalculateViewMatrix());

		glUseProgram(0);

		MainWindow->SpawBuffers();
	}

	delete MainWindow;

	return 0;
}