#include "App.hpp"

namespace lve {

	void FirstApp::run()
	{
		while (!m_LveWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}