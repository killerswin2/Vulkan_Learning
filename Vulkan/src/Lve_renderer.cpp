#include "Lve_renderer.hpp"

#include <stdexcept>
#include <array>
#include <cassert>

namespace lve {


	LveRenderer::LveRenderer(LveWindow& window, LveDevice& device)
		:m_LveWindow { window }, m_LveDevice{ device }
	{

		recreateSwapChain();
		createCommandBuffers();
	}

	LveRenderer::~LveRenderer()
	{
		freeCommmandBuffers();
	}


	void LveRenderer::recreateSwapChain()
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
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(m_LveSwapChain);
			m_LveSwapChain = std::make_unique<LveSwapChain>(m_LveDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*m_LveSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
		// createPipeline ?
	}

	void LveRenderer::createCommandBuffers()
	{
		m_CommandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

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

	void LveRenderer::freeCommmandBuffers()
	{
		vkFreeCommandBuffers(m_LveDevice.device(), m_LveDevice.getCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	VkCommandBuffer LveRenderer::beginFrame()
	{
		assert(!m_isFrameStarted && "Can't call beginFrame while already in progress");
		
		auto result = m_LveSwapChain->acquireNextImage(&m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		m_isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void LveRenderer::endFrame()
	{
		assert(m_isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = m_LveSwapChain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_LveWindow.WasWindowResized())
		{
			m_LveWindow.ResetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_isFrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_LveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_LveSwapChain->getFrameBuffer(m_CurrentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = m_LveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 0.01f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_LveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_LveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_LveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}