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
		glm::mat2 m_Transform{ 1.0f };
		glm::vec2 m_Offset;
		alignas(16) glm::vec3 m_Color;
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



	void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects)
	{
		m_LvePipeline->bind(commandBuffer);

		for (auto& obj : gameObjects)
		{
			obj.m_Transform2d.m_Rotation = glm::mod(obj.m_Transform2d.m_Rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.m_Offset = obj.m_Transform2d.m_Translation;
			push.m_Color = obj.m_Color;
			push.m_Transform = obj.m_Transform2d.mat2();

			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.m_Model->Bind(commandBuffer);
			obj.m_Model->Draw(commandBuffer);
		}
	}

}