#pragma once

#pragma once

#include "Lve_game_object.hpp"
#include "Lve_window.hpp"

namespace lve 
{
    class KeyboardMovementController 
    {
    public:
        struct KeyMappings 
        {
            int m_MoveLeft = GLFW_KEY_A;
            int m_MoveRight = GLFW_KEY_D;
            int m_MoveForward = GLFW_KEY_W;
            int m_MoveBackward = GLFW_KEY_S;
            int m_MoveUp = GLFW_KEY_E;
            int m_MoveDown = GLFW_KEY_Q;
            int m_LookLeft = GLFW_KEY_LEFT;
            int m_LookRight = GLFW_KEY_RIGHT;
            int m_LookUp = GLFW_KEY_UP;
            int m_LookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);

        KeyMappings m_Keys{};
        float m_MoveSpeed{ 3.f };
        float m_LookSpeed{ 1.5f };
    };
}