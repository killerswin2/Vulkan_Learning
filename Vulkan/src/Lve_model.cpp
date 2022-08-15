#include "Lve_model.hpp"

#include <cassert>
namespace lve
{
	LveModel::LveModel(LveDevice &device, const LveModel::Builder& builder)
		: m_LveDevice{ device }
	{
		CreateVertexBuffers(builder.m_Vertices);
		CreateIndexBuffers(builder.m_Indices);
	}

	LveModel::~LveModel()
	{
		vkDestroyBuffer(m_LveDevice.device(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_LveDevice.device(), m_VertexBufferMemory, nullptr);

		if (m_HasIndexBuffer)
		{
			vkDestroyBuffer(m_LveDevice.device(), m_IndexBuffer, nullptr);
			vkFreeMemory(m_LveDevice.device(), m_IndexBufferMemory, nullptr);
		}
	}

	void LveModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_LveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(m_LveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_LveDevice.device(), stagingBufferMemory);

		m_LveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertexBuffer,
			m_VertexBufferMemory
		);

		m_LveDevice.copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);
		vkDestroyBuffer(m_LveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_LveDevice.device(), stagingBufferMemory, nullptr);
	}

	void LveModel::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;
		
		if (!m_HasIndexBuffer)
		{
			return; 
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_LveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(m_LveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_LveDevice.device(), stagingBufferMemory);

		m_LveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer,
			m_IndexBufferMemory
		);

		m_LveDevice.copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);
		vkDestroyBuffer(m_LveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_LveDevice.device(), stagingBufferMemory, nullptr);
	}

	void LveModel::Draw(VkCommandBuffer commandBuffer)
	{
		if (m_HasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer,m_IndexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	void LveModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);

		if (m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, m_Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, m_Color);
		return attributeDescriptions;
	}
}