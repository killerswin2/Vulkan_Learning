#pragma once

#include "Lve_window.hpp"
#include "Lve_pipeline.hpp"
#include "Lve_swap_chain.hpp"
#include "Lve_device.hpp"
#include "Lve_model.hpp"
#include "Lve_game_object.hpp"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommmandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		LveWindow m_LveWindow{ WIDTH , HEIGHT, "Hello Vulkan!" };
		LveDevice m_LveDevice{ m_LveWindow };
		std::unique_ptr<LveSwapChain> m_LveSwapChain;
		std::unique_ptr<LvePipeline> m_LvePipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<LveGameObject> m_GameObjects;
	};
}