#include "App.hpp"
#include "Render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace lve {


	FirstApp::FirstApp()
	{
		loadGameObjects();

	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run()
	{
		RenderSystem renderSystem{m_LveDevice, m_LveRenderer.getSwapChainRenderPass()};
		while (!m_LveWindow.ShouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = m_LveRenderer.beginFrame()) 
			{
				m_LveRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, m_GameObjects);
				m_LveRenderer.endSwapChainRenderPass(commandBuffer);
				m_LveRenderer.endFrame();
			}

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
}