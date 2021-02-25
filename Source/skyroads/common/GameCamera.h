#pragma once
#include <include/glm.h>
#include <include/math.h>
#include <skyroads/objects3D/Player3D.h>
#include <skyroads/common/Constants.h>
#include <Core/Window/WindowObject.h>

namespace Game {
	class Camera {
		public:
			Camera();

			Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

			~Camera();

			void set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
			void moveForward(float distance);
			void translateForward(float distance);
			void translateUpword(float distance);
			void translateRight(float distance);
			void rotateFirstPerson_OX(float angle);
			void rotateFirstPerson_OY(float angle);
			void rotateFirstPerson_OZ(float angle);
			void rotateThirdPerson_OX(float angle);
			void rotateThirdPerson_OY(float angle);
			void rotateThirdPerson_OZ(float angle);

			glm::mat4 getViewMatrix();
			glm::vec3 getTargetPosition();

			void update(Player3D& target, WindowObject* window, float deltaTimeSeconds);

		public:
			glm::mat4 projectionMatrix;

			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 right;
			glm::vec3 up;

			float fieldOfView;
			glm::vec3 animationOffset;
			float angleYZDegrees;
			bool thirdPerson;
			bool free;
		};
}