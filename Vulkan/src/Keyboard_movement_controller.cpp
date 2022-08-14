#include "keyboard_movement_controller.hpp"

// std
#include <limits>

namespace lve {

    void KeyboardMovementController::moveInPlaneXZ(
        GLFWwindow* window, float dt, LveGameObject& gameObject) {
        glm::vec3 rotate{ 0 };
        if (glfwGetKey(window, m_Keys.m_LookRight) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, m_Keys.m_LookLeft) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, m_Keys.m_LookUp) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, m_Keys.m_LookDown) == GLFW_PRESS) rotate.x -= 1.f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.m_Transform.m_Rotation += m_LookSpeed * dt * glm::normalize(rotate);
        }

        // limit pitch values between about +/- 85ish degrees
        gameObject.m_Transform.m_Rotation.x = glm::clamp(gameObject.m_Transform.m_Rotation.x, -1.5f, 1.5f);
        gameObject.m_Transform.m_Rotation.y = glm::mod(gameObject.m_Transform.m_Rotation.y, glm::two_pi<float>());

        float yaw = gameObject.m_Transform.m_Rotation.y;
        const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, -1.f, 0.f };

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, m_Keys.m_MoveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, m_Keys.m_MoveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, m_Keys.m_MoveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, m_Keys.m_MoveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, m_Keys.m_MoveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, m_Keys.m_MoveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.m_Transform.m_Translation += m_MoveSpeed * dt * glm::normalize(moveDir);
        }
    }
}