#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve 
{
	class LveWindow 
	{
	public:
		LveWindow(int width, int height, std::string name);
		~LveWindow();
		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

		LveWindow(const LveWindow&) = delete;
		LveWindow& operator=(const LveWindow&) = delete;

	private:
		void InitWindow();
		const int m_Width;
		const int m_Height;

		std::string m_WindowName;
		GLFWwindow* m_Window;
		

	};
}