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
			glm::mat4 mat4() {
			const float c3 = glm::cos(m_Rotation.z);
			const float s3 = glm::sin(m_Rotation.z);
			const float c2 = glm::cos(m_Rotation.x);
			const float s2 = glm::sin(m_Rotation.x);
			const float c1 = glm::cos(m_Rotation.y);
			const float s1 = glm::sin(m_Rotation.y);
			return glm::mat4{
				{
					m_Scale.x * (c1 * c3 + s1 * s2 * s3),
					m_Scale.x * (c2 * s3),
					m_Scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					m_Scale.y * (c3 * s1 * s2 - c1 * s3),
					m_Scale.y * (c2 * c3),
					m_Scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					m_Scale.z * (c2 * s1),
					m_Scale.z * (-s2),
					m_Scale.z * (c1 * c2),
					0.0f,
				},
				{m_Translation.x, m_Translation.y, m_Translation.z, 1.0f} };
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
		TransformComponent m_Transform{};

	private:
		LveGameObject(id_t ObjId)
			: m_id{ObjId} {}
		id_t m_id;
	};
}
