#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 StartPosition, glm::vec3 StartUp, GLfloat StartYaw, GLfloat StartPitch, GLfloat StartMoveSpeed, GLfloat StartTurnSpeed);
	
	void KeyControl(bool* keys, GLfloat DeltaTime);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	glm::mat4 CalculateViewMatrix();

	glm::vec3 GetCameraPosition();
	glm::vec3 GetCameraDirection();
	
	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 WorldUp; // Direction of the world

	GLfloat yaw;
	GLfloat pitch;
	//Dont need roll

	GLfloat MovementSpeed;
	GLfloat TurnSpeed;

	void UpdateCamera();
};

