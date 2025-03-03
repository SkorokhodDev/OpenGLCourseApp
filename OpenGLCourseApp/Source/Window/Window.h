#pragma once

#include <stdio.h>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Window
{
public:
	Window();
	Window(GLint WindowWidth, GLint WindowHeight);
	~Window();

	int Initialize();

	GLfloat GetBufferWidth() { return (GLfloat)bufferWidth; }
	GLfloat GetBufferHeight() { return (GLfloat)bufferHeight; }

	bool GetShouldClose() { return glfwWindowShouldClose(MainWindow); }
	void SpawBuffers() { glfwSwapBuffers(MainWindow); }

	bool* GetKeys() { return keys; }
	GLfloat GetXChange();
	GLfloat GetYChange();

private:
	GLFWwindow* MainWindow;

	GLint width, height;
	GLint bufferWidth;
	GLint bufferHeight;

	bool keys[1024];

	void CreateCallBack();

	GLfloat lastX;
	GLfloat lastY;
	// differense between last coordinates (lastX,Y) and current coords
	//then we understand how to move camera, when we calculate X-Ychange
	GLfloat Xchange;
	GLfloat Ychange;
	
	bool bMouseFirstMoved;

	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
};

