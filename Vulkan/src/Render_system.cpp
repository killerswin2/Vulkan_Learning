#include "Render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace lve {

	struct SimplePushConstantData
	{
		glm::mat4 m_Transform{ 1.0f };
		glm::mat4 m_NormalMatrix{ 1.0f };
	};

	RenderSystem::RenderSystem(LveDevice& device, VkRenderPass renderPass)
		: m_LveDevice{ device }
	{
		
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(m_LveDevice.device(), m_PipelineLayout, nullptr);
	}


	void RenderSystem::createPipelineLayout()
	{

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelinelayoutInfo{};
		pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelinelayoutInfo.setLayoutCount = 0;
		pipelinelayoutInfo.pSetLayouts = nullptr;
		pipelinelayoutInfo.pushConstantRangeCount = 1;
		pipelinelayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_LveDevice.device(), &pipelinelayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipelinelayout!");
		}
	}
	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipeLineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipeLineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_LvePipeline = std::make_unique<LvePipeline>(
			m_LveDevice,
			"shaders\\Simple_Shader.vert.spv",
			"shaders\\Simple_Shader.frag.spv",
			pipelineConfig
			);
	}



	void RenderSystem::renderGameObjects(FrameInfo & frameInfo, std::vector<LveGameObject>& gameObjects)
	{
		m_LvePipeline->bind(frameInfo.m_CommandBuffer);

		auto projectionView = frameInfo.m_Camera.getProjection() * frameInfo.m_Camera.getView();

		for (auto& obj : gameObjects)
		{

			SimplePushConstantData push{};
			auto modelMatrix = obj.m_Transform.mat4();
			push.m_Transform = projectionView * modelMatrix;
			push.m_NormalMatrix = obj.m_Transform.normalMatrix();

			vkCmdPushConstants(frameInfo.m_CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.m_Model->Bind(frameInfo.m_CommandBuffer);
			obj.m_Model->Draw(frameInfo.m_CommandBuffer);
		}
	}

}