#pragma once

#include "Lve_window.hpp"
#include "Lve_pipeline.hpp"
#include "Lve_device.hpp"

namespace lve
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();
	private:
		LveWindow m_LveWindow{ WIDTH , HEIGHT, "Hello Vulkan!" };
		LveDevice m_LveDevice{ m_LveWindow };
		LvePipeline m_LvePipeline { m_LveDevice, "shaders\\Simple_Shader.vert.spv", "shaders\\Simple_Shader.frag.spv", LvePipeline::DefaultPipeLineConfigInfo(WIDTH, HEIGHT)};
	};
}