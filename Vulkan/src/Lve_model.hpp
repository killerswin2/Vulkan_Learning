#pragma once

#include "Lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace lve
{
	class LveModel
	{
	public:

		struct Vertex
		{
			glm::vec3 m_Position;
			glm::vec3 m_Color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Builder
		{
			std::vector<Vertex> m_Vertices{};
			std::vector<uint32_t> m_Indices{};
		};

		LveModel(LveDevice &device, const LveModel::Builder &builder);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel &operator=(const LveModel&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		LveDevice&		m_LveDevice;
		VkBuffer		m_VertexBuffer;
		VkDeviceMemory	m_VertexBufferMemory;
		uint32_t		m_VertexCount;

		bool m_HasIndexBuffer = false;

		VkBuffer		m_IndexBuffer;
		VkDeviceMemory	m_IndexBufferMemory;
		uint32_t		m_IndexCount;

	};

}