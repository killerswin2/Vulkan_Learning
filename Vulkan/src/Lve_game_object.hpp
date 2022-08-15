#pragma once

#include "Lve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace lve
{
	struct TransformComponent
	{
		glm::vec3 m_Translation {}; // position offset
		glm::vec3 m_Scale{1.0f, 1.0f, 1.0f};
		glm::vec3 m_Rotation;


		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
		TransformComponent m_Transform{};

	private:
		LveGameObject(id_t ObjId)
			: m_id{ObjId} {}
		id_t m_id;
	};
}
