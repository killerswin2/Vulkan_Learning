#include "App.h"

namespace lve {

	void FirstApp::run()
	{
		while (!m_LveWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}