#pragma once

#include "Lve_model.hpp";

#include <memory>

namespace lve
{
	struct Transform2dComponent
	{
		glm::vec2 m_Translation{}; // position offset
		glm::vec2 m_Scale{1.0f, 1.0f};
		float m_Rotation;

		glm::mat2 mat2() 
		{ 
			const float s = glm::sin(m_Rotation);
			const float c = glm::cos(m_Rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {m_Scale.x, 0.0f}, {0.0f, m_Scale.y} };
			return rotMatrix * scaleMat;
		}
		
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;

		static LveGameObject createGameObject() 
		{
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		const id_t GetId() { return m_id; };

		std::shared_ptr<LveModel> m_Model{};
		glm::vec3 m_Color{};
		Transform2dComponent m_Transform2d{};

	private:
		LveGameObject(id_t ObjId)
			: m_id{ObjId} {}
		id_t m_id;
	};
}
