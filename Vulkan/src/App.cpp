#include "App.hpp"

#include <stdexcept>
#include <array>

namespace lve {

	FirstApp::FirstApp()
	{
		loadModels();
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
			drawFrame();
		}

		vkDeviceWaitIdle(m_LveDevice.device());
	}
	void FirstApp::loadModels()
	{
		std::vector<LveModel::Vertex> vertices{
			{{ 0.0f, -0.5f}},
			{{ 0.5f,  0.5f}},
			{{-0.5f,  0.5f}}
		};

		m_LveModel = std::make_unique<LveModel>(m_LveDevice, vertices);
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
		m_CommandBuffers.resize(m_LveSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_LveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_LveDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffer");
		}

		for (int i = 0; i < m_CommandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_LveSwapChain.getRenderPass();
			renderPassInfo.framebuffer = m_LveSwapChain.getFrameBuffer(i);
			renderPassInfo.renderArea.offset = { 0, 0 };

			renderPassInfo.renderArea.extent = m_LveSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f};
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_LvePipeline->bind(m_CommandBuffers[i]);
			m_LveModel->Bind(m_CommandBuffers[i]);
			m_LveModel->Draw(m_CommandBuffers[i]);

			vkCmdEndRenderPass(m_CommandBuffers[i]);
			if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer");
			}
		}
	}
	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = m_LveSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = m_LveSwapChain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}