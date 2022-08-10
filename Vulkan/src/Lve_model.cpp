#include "Lve_model.hpp"

#include <cassert>
namespace lve
{
	LveModel::LveModel(LveDevice &device, const std::vector<Vertex>& vertices)
		: m_LveDevice{ device }
	{
		CreateVertexBuffers(vertices);
	}

	LveModel::~LveModel()
	{
		vkDestroyBuffer(m_LveDevice.device(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_LveDevice.device(), m_VertexBufferMemory, nullptr);
	}

	void LveModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		m_LveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_VertexBuffer,
			m_VertexBufferMemory
			);
		void* data;
		vkMapMemory(m_LveDevice.device(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_LveDevice.device(), m_VertexBufferMemory);
	}

	void LveModel::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}

	void LveModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	}

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		return attributeDescriptions;
	}
}