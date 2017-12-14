#pragma once
#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* window;

public:
	Window();
	int init(int width,int length,char[12]);
	~Window();
	GLFWwindow* get();
};

