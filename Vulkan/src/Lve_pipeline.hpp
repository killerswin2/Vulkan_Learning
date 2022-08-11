#pragma once

#include "Lve_device.hpp"
#include <string>
#include <vector>

namespace lve 
{
	struct PipeLineConfigInfo
	{
		PipeLineConfigInfo(const PipeLineConfigInfo&) = delete;
		PipeLineConfigInfo& operator=(const PipeLineConfigInfo&) = delete;
		
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStatesEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class LvePipeline
	{
	public:
		LvePipeline(LveDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipeLineConfigInfo& configInfo);
		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete;
		void operator=(const LvePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void DefaultPipeLineConfigInfo(PipeLineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipeLineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule);

		LveDevice& m_LveDevice;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}