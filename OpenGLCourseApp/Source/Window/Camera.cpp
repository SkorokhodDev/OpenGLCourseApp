#include "Camera.h"


Camera::Camera()
{
}

Camera::Camera(glm::vec3 StartPosition, glm::vec3 StartUp,
	GLfloat StartYaw, GLfloat StartPitch,GLfloat StartMoveSpeed, GLfloat StartTurnSpeed) 
	: position(StartPosition), WorldUp(StartUp), yaw(StartYaw), pitch(StartPitch), 
	MovementSpeed(StartMoveSpeed), TurnSpeed(StartTurnSpeed)
{
	// Initialize WorldUp, not the "up" variable
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	
	UpdateCamera();
}

void Camera::KeyControl(bool* keys, GLfloat DeltaTime)
{
	GLfloat Velocity = MovementSpeed * DeltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * Velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= front * Velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		position += right * Velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		position -= right * Velocity;
	}
}

void Camera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= TurnSpeed;
	yChange *= TurnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	UpdateCamera();
}

glm::mat4 Camera::CalculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::GetCameraPosition()
{
	return position;
}

glm::vec3 Camera::GetCameraDirection()
{
	return glm::normalize(front);
}

Camera::~Camera()
{
}

void Camera::UpdateCamera()
{
	// Векторная математика
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, WorldUp));
	up = glm::normalize(glm::cross(right, front));
	
}
