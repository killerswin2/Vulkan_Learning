#include "App.hpp"

#include <stdexcept>

namespace lve {

	FirstApp::FirstApp()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(m_LveDevice.device(), m_PipelineLayout, nullptr);
	}

	void FirstApp::run()
	{
		while (!m_LveWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
	void FirstApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelinelayoutInfo{};
		pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelinelayoutInfo.setLayoutCount = 0;
		pipelinelayoutInfo.pSetLayouts = nullptr;
		pipelinelayoutInfo.pushConstantRangeCount = 0;
		pipelinelayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_LveDevice.device(), &pipelinelayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipelinelayout!");
		}
	}
	void FirstApp::createPipeline()
	{
		PipeLineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipeLineConfigInfo(pipelineConfig, m_LveSwapChain.width(), m_LveSwapChain.height());
		pipelineConfig.renderPass = m_LveSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_LvePipeline = std::make_unique<LvePipeline>(
			m_LveDevice, 
			"shaders\\Simple_Shader.vert.spv", 
			"shaders\\Simple_Shader.frag.spv",
			pipelineConfig
		);
	}
	void FirstApp::createCommandBuffers()
	{
	}
	void FirstApp::drawFrame()
	{
	}
}