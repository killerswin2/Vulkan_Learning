#pragma once

#include "Lve_camera.hpp"

#include <vulkan/vulkan.h>

namespace lve
{
	struct FrameInfo
	{
		int m_FrameIndex;
		float m_FrameTime;
		VkCommandBuffer m_CommandBuffer;
		LveCamera& m_Camera;
	};
}