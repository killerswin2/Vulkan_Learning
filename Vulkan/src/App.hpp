#pragma once

#include "Lve_window.hpp"
#include "Lve_pipeline.hpp"

namespace lve
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGTH = 600;

		void run();
	private:
		LveWindow m_LveWindow{ WIDTH , HEIGTH, "Hello Vulkan!" };
		LvePipeline m_LvePipeline {"shaders\\Simple_Shader.vert.spv", "shaders\\Simple_Shader.frag.spv"};
	};
}