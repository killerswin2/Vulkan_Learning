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

		LveWindow(const LveWindow&) = delete;
		LveWindow& operator=(const LveWindow&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
		bool WasWindowResized() { return m_FrameBufferResized; }
		void ResetWindowResizedFlag() { m_FrameBufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return m_Window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

	private:
		static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);
		void InitWindow();
		int m_Width;
		int m_Height;
		bool m_FrameBufferResized = false;

		std::string m_WindowName;
		GLFWwindow* m_Window;
		

	};
}