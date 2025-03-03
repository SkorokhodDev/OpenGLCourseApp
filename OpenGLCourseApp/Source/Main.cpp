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


std::vector<Mesh*> MeshList; 
std::vector<Shader> ShaderList; 

DirectionalLight MainLight;
PointLight PointLights[MAX_POINT_LIGHTS];
SpotLight SpotLights[MAX_SPOT_LIGHTS];

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

		in0 += normalOffset; in1 += normalOffset, in2 += normalOffset;
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

	unsigned int FloorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};


	GLfloat FloorVertices[] = {
		//	x	 y		z		u	  v			nx	  ny	nz
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	CalcAverageNormals(indices, sizeof(indices) / sizeof(indices[0]), 
		vertices, sizeof(vertices) / sizeof(vertices[0]), 8, 5);

	// Creating Mesh 1
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, sizeof(vertices) / sizeof(vertices[0]), sizeof(indices) / sizeof(indices[0]));
	MeshList.push_back(obj1);

	Mesh* Floor = new Mesh();
	Floor->CreateMesh(FloorVertices, FloorIndices, 
		sizeof(FloorVertices) / sizeof(FloorVertices[0]), sizeof(FloorIndices) / sizeof(FloorIndices[0]));
	MeshList.push_back(Floor);

}

void CreateShaders()
{
	// Create shader1
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	ShaderList.push_back(*shader1);
}

int main()
{
	Window* MainWindow = new Window(1300, 700);
	MainWindow->Initialize();

	CreateShaders();
	CreateObjects();

	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 2.0f, 0.2f);

	///////////////////// Load Texture 
	Texture Tex_Brick = Texture("Resources/Textures/brick.png");
	Tex_Brick.LoadTexture(true);
	Texture Tex_Dirt = Texture("Resources/Textures/dirt.png");
	Tex_Dirt.LoadTexture(true);
	Texture Tex_Plain = Texture("Resources/Textures/plain.png");
	Tex_Plain.LoadTexture(true);
	
	//////////////////// Materials
	Material ShinyMaterial = Material(1.0f, 32);
	Material DullMaterial = Material(0.3f, 4);

	///////////////////// L I G H T I N G
	//Initialize DirectionalLight
	MainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f);
	// Initialize PointLights
	unsigned int PointLightCount = 0;
	PointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
								0.0f, 0.1f,
								-4.0f, 0.0f, 0.0f,
								0.3f, 0.1f, 0.1f);
	PointLightCount++;
	PointLights[1] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 0.1f,
								0.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	PointLightCount++;
	
	// Initialize SpotLights
	unsigned int SpotLightCount = 0;
	SpotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
							0.1f, 1.0f,
							0.0f, 0.0f, 0.0f,
							0.0f, -1.0f, 0.0f,
							1.0f, 0.0f, 0.0f, 20.0f);
	SpotLightCount++;
	SpotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
								1.0f, 3.0f,
								3.0f, 0.0f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f, 20.0f);
	SpotLightCount++;

	////////////////////// Models
	Model Xwing= Model();
	Xwing.LoadModel("Resources/Models/x-wing.obj");

	Model BlackHawk = Model();

	// Locations of our uniforms
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	GLuint unifromSpecularIntensity = 0, uniformShininess = 0, uniformEyePosition = 0;

	glm::mat4 projection = glm::perspective(45.0f, MainWindow->GetBufferWidth() / MainWindow->GetBufferHeight(), 0.1f, 100.0f);

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

		// Clear window 
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// our rendering
		ShaderList[0].UseShader();
		uniformModel = ShaderList[0].GetModelLocation();
		uniformProjection = ShaderList[0].GetProjectionLocation();
		uniformView = ShaderList[0].GetViewLocation();
		uniformEyePosition = ShaderList[0].GetEyePositionLocation();
		unifromSpecularIntensity = ShaderList[0].GetSpecularIntesityLocation();
		uniformShininess = ShaderList[0].GetShininessLocation();

		glm::vec3 LowerLight = camera.GetCameraPosition();
		LowerLight.y -= 0.01f;
		SpotLights[0].SetFlash(LowerLight, camera.GetCameraDirection());

		ShaderList[0].SetDirectionalLight(&MainLight);
		ShaderList[0].SetPointLights(PointLights, PointLightCount);
		ShaderList[0].SetSpotLights(SpotLights, SpotLightCount);


		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);

		/////////////////////////////////// MeshList[0]
		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		// attach our projection matrix to shader uniform variable
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tex_Brick.UseTexture();
		ShinyMaterial.UseMaterial(unifromSpecularIntensity, uniformShininess); // set Material class data to the uniformValues
		MeshList[0]->RenderMesh(); // render, where we actually calling shaders

		/////////////// FLooor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tex_Dirt.UseTexture();
		DullMaterial.UseMaterial(unifromSpecularIntensity, uniformShininess); 
		MeshList[1]->RenderMesh(); 

		//////////////////// Xwing
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Tex_Dirt.UseTexture();
		ShinyMaterial.UseMaterial(unifromSpecularIntensity, uniformShininess);
		Xwing.RenderModel();


		glUseProgram(0);
		///////////////////////////////////

		MainWindow->SpawBuffers();
	}

	delete MainWindow;

	return 0;
}