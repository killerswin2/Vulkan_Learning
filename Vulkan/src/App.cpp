#include "App.hpp"

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
		glm::mat2 m_Transform{1.0f};
		glm::vec2 m_Offset;
		alignas(16) glm::vec3 m_Color;
	};

	FirstApp::FirstApp()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
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
	void FirstApp::loadGameObjects()
	{
		std::vector<LveModel::Vertex> vertices{
			{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto lveModel = std::make_shared<LveModel>(m_LveDevice, vertices);

		auto triangle = LveGameObject::createGameObject();
		triangle.m_Model = lveModel;
		triangle.m_Color = { 0.1f, 0.8f, 0.1f };
		triangle.m_Transform2d.m_Translation.x = 0.2f;
		triangle.m_Transform2d.m_Scale = { 2.f, 0.5f };
		triangle.m_Transform2d.m_Rotation = .25f * glm::two_pi<float>();

		m_GameObjects.push_back(std::move(triangle));

	}

	void FirstApp::createPipelineLayout()
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
	void FirstApp::createPipeline()
	{
		assert(m_LveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipeLineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipeLineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_LveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_LvePipeline = std::make_unique<LvePipeline>(
			m_LveDevice, 
			"shaders\\Simple_Shader.vert.spv", 
			"shaders\\Simple_Shader.frag.spv",
			pipelineConfig
		);
	}

	void FirstApp::recreateSwapChain()
	{
		auto extent = m_LveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_LveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_LveDevice.device());

		if (m_LveSwapChain == nullptr)
		{
			m_LveSwapChain = std::make_unique<LveSwapChain>(m_LveDevice, extent);
		}
		else {
			m_LveSwapChain = std::make_unique<LveSwapChain>(m_LveDevice, extent, std::move(m_LveSwapChain));
			if (m_LveSwapChain->imageCount() != m_CommandBuffers.size())
			{
				freeCommmandBuffers();
				createCommandBuffers();
			}
		}
		createPipeline();
	}

	void FirstApp::createCommandBuffers()
	{
		m_CommandBuffers.resize(m_LveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_LveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_LveDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffer");
		}
	}

	void FirstApp::freeCommmandBuffers()
	{
		vkFreeCommandBuffers(m_LveDevice.device(), m_LveDevice.getCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void FirstApp::recordCommandBuffer(int imageIndex)
	{

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_LveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_LveSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = m_LveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 0.01f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_LveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_LveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_LveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(m_CommandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		m_LvePipeline->bind(commandBuffer);

		for (auto& obj : m_GameObjects)
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

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = m_LveSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = m_LveSwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_LveWindow.WasWindowResized())
		{
			m_LveWindow.ResetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}