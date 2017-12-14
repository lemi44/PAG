#include "Window.h"



Window::Window()
{
}

int Window::init(int width, int length, char name[12])
{

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, length, name, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	return 0;
}

GLFWwindow* Window::get()
{
	return window;
}

Window::~Window()
{
}
