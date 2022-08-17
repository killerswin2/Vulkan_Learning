#include "App.hpp"
#include "Keyboard_movement_controller.hpp"
#include "Render_system.hpp"
#include "Lve_camera.hpp"
#include "Lve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <numeric>
#include <cassert>

namespace lve {

	struct GlobalUbo
	{
		alignas(16) glm::mat4 m_projectionView{ 1.0f };
		alignas(16) glm::vec3 m_LightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	FirstApp::FirstApp()
	{
		loadGameObjects();

	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run()
	{
		auto minOffsetAlignment = std::lcm(
			m_LveDevice.properties.limits.minUniformBufferOffsetAlignment,
			m_LveDevice.properties.limits.nonCoherentAtomSize);

		LveBuffer globalUboBuffer{ 
			m_LveDevice, 
			sizeof(GlobalUbo),
			LveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			minOffsetAlignment
		};

		globalUboBuffer.map();

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
				int frameIndex = m_LveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// update
				GlobalUbo ubo{};
				ubo.m_projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				// render
				m_LveRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, m_GameObjects);
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