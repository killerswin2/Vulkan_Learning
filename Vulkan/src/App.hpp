#pragma once

#include "Lve_window.hpp"
#include "Lve_device.hpp"
#include "Lve_game_object.hpp"
#include "Lve_renderer.hpp"

#include <memory>
#include <vector>

namespace lve
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
	private:
		void loadGameObjects();


		LveWindow m_LveWindow{ WIDTH , HEIGHT, "Hello Vulkan!" };
		LveDevice m_LveDevice{ m_LveWindow };
		LveRenderer m_LveRenderer{m_LveWindow, m_LveDevice};

		std::vector<LveGameObject> m_GameObjects;
	};
}