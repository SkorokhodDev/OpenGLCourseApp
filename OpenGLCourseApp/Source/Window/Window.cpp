#include "Window.h"
#include <cstring>

Window::Window() : width(800), height(600)
{
	lastX = lastY = 0;
	bufferWidth = bufferHeight = 0;
	MainWindow = nullptr;
	Xchange = Ychange = 0.0f;
	bMouseFirstMoved = false;
	memset(keys, 0, sizeof(keys) / sizeof(keys[0]));
}

Window::Window(GLint WindowWidth, GLint WindowHeight) : Window()
{
	width = WindowWidth;
	height = WindowHeight;
}

Window::~Window()
{
	glfwDestroyWindow(MainWindow);
	glfwTerminate();
}

int Window::Initialize()
{
	//Initialize  GLFW
	if (!glfwInit())
	{
		printf("GLFW INITIALIZATION FAILED!");
		glfwTerminate();
		return -1;
	}

	// Setup GLFW Window Properties
	// OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility (no deprecated code)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow forward comatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	MainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!MainWindow)
	{
		printf("GLFW Window creation failed!");
		glfwTerminate();
		return -1;
	}

	//Get Buffer size information
	glfwGetFramebufferSize(MainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(MainWindow);

	//Create keys + mouse inputs
	CreateCallBack(); 
	glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor in game

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed");
		glfwDestroyWindow(MainWindow);
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create viewport and set viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	//Tell glfw which window calling handleKeys
	glfwSetWindowUserPointer(MainWindow, this);

	return 0;
}

GLfloat Window::GetXChange()
{
	GLfloat DeltaX = Xchange;
	Xchange = 0;
	return DeltaX;
}

GLfloat Window::GetYChange()
{
	GLfloat DeltaY = Ychange;
	Ychange = 0;
	return DeltaY;
}

void Window::CreateCallBack()
{
	glfwSetKeyCallback(MainWindow, HandleKeys);
	glfwSetCursorPosCallback(MainWindow, HandleMouse);
}

void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* TheWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < sizeof(keys) / sizeof(keys[0]))
	{
		if (action == GLFW_PRESS)
		{
			TheWindow->keys[key] = true;
			//printf("Pressed: %d\n", key);
		}
		else if(action == GLFW_RELEASE)
		{
			TheWindow->keys[key] = false;
			//printf("Released: %d\n", key);
		}
	}
}

void Window::HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* TheWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (TheWindow->bMouseFirstMoved)
	{
		TheWindow->lastX = xPos;
		TheWindow->lastY = yPos;
		TheWindow->bMouseFirstMoved = false;
	}

	TheWindow->Xchange = xPos - TheWindow->lastX;
	TheWindow->Ychange = TheWindow->lastY - yPos;

	//Ready for the next time when mouse is moved
	TheWindow->lastX = xPos;
	TheWindow->lastY = yPos;
}
