#include "Lve_window.hpp"
#include "stdexcept"

namespace lve 
{

	LveWindow::LveWindow(int width, int height, std::string name) 
		: m_Width{ width }, m_Height{ height }, m_WindowName{ name }
	{
		InitWindow();
	}
	LveWindow::~LveWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void LveWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
	}

	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

}