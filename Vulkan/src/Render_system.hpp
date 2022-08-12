#pragma once

#include "Lve_pipeline.hpp"
#include "Lve_device.hpp"
#include "Lve_game_object.hpp"

#include <memory>
#include <vector>

namespace lve
{
	class RenderSystem
	{
	public:

		RenderSystem(LveDevice& device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;
		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& m_LveDevice;

		std::unique_ptr<LvePipeline> m_LvePipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}