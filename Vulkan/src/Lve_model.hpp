#pragma once

#include "Lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>


namespace lve
{
	class LveModel
	{
	public:

		struct Vertex
		{
			glm::vec3 m_Position{};
			glm::vec3 m_Color{};
			glm::vec3 m_Normal{};
			glm::vec2 m_uv{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return	m_Position	== other.m_Position && 
						m_Color		== other.m_Color &&
						m_Normal	== other.m_Normal &&
						m_uv		== other.m_uv;
			}
		};


		struct Builder
		{
			std::vector<Vertex> m_Vertices{};
			std::vector<uint32_t> m_Indices{};

			void loadModel(const std::string& filepath);
		};

		LveModel(LveDevice &device, const LveModel::Builder &builder);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel &operator=(const LveModel&) = delete;

		static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);

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