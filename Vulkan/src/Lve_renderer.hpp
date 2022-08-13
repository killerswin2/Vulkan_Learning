#pragma once

#include "Lve_window.hpp"
#include "Lve_swap_chain.hpp"
#include "Lve_device.hpp"


#include <memory>
#include <vector>
#include <cassert>

namespace lve
{
	class LveRenderer
	{
	public:

		LveRenderer(LveWindow& window, LveDevice& device);
		~LveRenderer();

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return m_LveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return m_LveSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(m_isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		int getFrameIndex() const 
		{
			assert(m_isFrameStarted && "Cannot get frame index when frame is not in progress");
			return m_CurrentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommmandBuffers();
		void recreateSwapChain();

		LveWindow& m_LveWindow;
		LveDevice& m_LveDevice;
		std::unique_ptr<LveSwapChain> m_LveSwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex;
		bool m_isFrameStarted = false;
	};
}