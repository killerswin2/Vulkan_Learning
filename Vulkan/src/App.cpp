#include "App.hpp"
#include "Keyboard_movement_controller.hpp"
#include "Render_system.hpp"
#include "Lve_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
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
        LveCamera camera{};
        camera.setViewTarget(glm::vec3{ 0.0f, 0.0f, 5.0f }, glm::vec3{ 0.0f, 0.0f, 2.5f });
        
        auto viewerObject = LveGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto curretTime = std::chrono::high_resolution_clock::now();

		while (!m_LveWindow.ShouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - curretTime).count();
            curretTime = newTime;

            cameraController.moveInPlaneXZ(m_LveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.m_Transform.m_Translation, viewerObject.m_Transform.m_Rotation);

            float aspect = m_LveRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 1000.0f);

			if (auto commandBuffer = m_LveRenderer.beginFrame()) 
			{
				m_LveRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, m_GameObjects, camera);
				m_LveRenderer.endSwapChainRenderPass(commandBuffer);
				m_LveRenderer.endFrame();
			}

		}

		vkDeviceWaitIdle(m_LveDevice.device());
	}


	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(m_LveDevice, "models/flat_vase.obj");
		auto gameObj = LveGameObject::createGameObject();
		gameObj.m_Model = lveModel;
		gameObj.m_Transform.m_Translation = { 0.0f , 0.5f, 1.5f };
		gameObj.m_Transform.m_Scale = glm::vec3{ 3.0f };

		m_GameObjects.push_back(std::move(gameObj));
	}
}