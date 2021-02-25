#pragma once
#include <skyroads/common/GameCamera.h>

Game::Camera::Camera() {
	position = glm::vec3(0, 2, 5);
	forward  = glm::vec3(0, 0, -1);
	up		 = glm::vec3(0, 1, 0);
	right	 = glm::vec3(1, 0, 0);

	fieldOfView = Constants::initialFieldOfView;
	animationOffset = glm::vec3(0);
	free = false;
	thirdPerson = true;
	angleYZDegrees = 0;
}

Game::Camera::Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up) {
	set(position, center, up);
	animationOffset = glm::vec3(0);
	free = false;
	thirdPerson = true;
	angleYZDegrees = 0;
}

Game::Camera::~Camera() {
}

void Game::Camera::set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up) {
	this->position = position;
	forward  = glm::normalize(center - position);
	right	 = glm::cross(forward, up);
	this->up = glm::cross(right, forward);
}

void Game::Camera::moveForward(float distance) {
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	/* Movement will keep the camera at the same height always
	Example: If you rotate up/down your head and walk forward
	you will still keep the same relative distance (height) to
	the ground!
	Translate the camera using the DIR vector computed from
	forward. */
	position += dir * distance;
}

void Game::Camera::translateForward(float distance) {
	// Translate the camera using the "forward" vector
	position += glm::normalize(forward) * distance;
}

void Game::Camera::translateUpword(float distance) {
	// Translate the camera using the up vector
	glm::vec3 dir = glm::normalize(glm::vec3(0, up.y, 0));
	position += dir * distance;
}

void Game::Camera::translateRight(float distance) {
	/* Translate the camera using the "right" vector.
	Usually translation using camera "right' is not very useful
	because if the camera is rotated around the "forward" vector 
	translation over the right direction will have an undesired
	effect; the camera will get closer or farther from the ground

	Using the projected right vector (onto the ground plane) makes
	more sense because we will keep the same distance from the
	ground plane.
	*/
	glm::vec3 pRight = glm::normalize(glm::vec3(right.x, 0, right.z));
	position += pRight * distance;
}

void Game::Camera::rotateFirstPerson_OX(float angle) {
	// Compute the new "forward" and "up" vectors
	// Attention! Don't forget to normalize the vectors
	// Use glm::rotate()
	glm::vec4 tmp_vec4 = glm::vec4(forward, 1.0);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, right);
	tmp_vec4 = model * tmp_vec4;
	glm::vec3 newForward = glm::normalize(glm::vec3(tmp_vec4));

	glm::vec3 newUp = glm::cross(right, newForward);
	if (newUp.y >= 0) {
		forward = newForward;
		up = newUp;
	}
}

void Game::Camera::rotateFirstPerson_OY(float angle) {
	// Compute the new "forward", "up" and "right" vectors
	// Don't forget to normalize the vectors
	// Use glm::rotate()
	glm::vec4 tmp_vec4 = glm::vec4(forward, 1.0);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
	tmp_vec4 = model * tmp_vec4;
	forward = glm::normalize(glm::vec3(tmp_vec4));

	tmp_vec4 = glm::vec4(right, 1.0);
	model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
	tmp_vec4 = model * tmp_vec4;
	right = glm::normalize(glm::vec3(tmp_vec4));
	up = glm::cross(right, forward);
}

void Game::Camera::rotateFirstPerson_OZ(float angle) {
	// Compute the new Right and Up, Forward stays the same
	// Don't forget to normalize the vectors
	glm::vec4 tmp_vec4 = glm::vec4(right, 1.0);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, forward);
	tmp_vec4 = model * tmp_vec4;
	right = glm::normalize(glm::vec3(tmp_vec4));
	up = glm::cross(right, forward);
}

void Game::Camera::rotateThirdPerson_OX(float angle) {
	// Rotate the camera in Third Person mode - OX axis
	// Use Constants::distanceToTarget as translation distance
	translateForward(Constants::thirdPersonDistanceToTarget);
	rotateFirstPerson_OX(angle);
	translateForward(-Constants::thirdPersonDistanceToTarget);
}

void Game::Camera::rotateThirdPerson_OY(float angle) {
	// Rotate the camera in Third Person mode - OY axis
	translateForward(Constants::thirdPersonDistanceToTarget);
	rotateFirstPerson_OY(angle);
	translateForward(-Constants::thirdPersonDistanceToTarget);
}

void Game::Camera::rotateThirdPerson_OZ(float angle) {
	// Rotate the camera in Third Person mode - OZ axis
	translateForward(Constants::thirdPersonDistanceToTarget);
	rotateFirstPerson_OZ(angle);
	translateForward(-Constants::thirdPersonDistanceToTarget);
}

glm::mat4 Game::Camera::getViewMatrix() {
	// Returns the View Matrix
	return glm::lookAt(position, position + forward, up);
}

glm::vec3 Game::Camera::getTargetPosition() {
	return position + forward * Constants::thirdPersonDistanceToTarget;
}

void Game::Camera::update(Player3D& target, WindowObject* window, float deltaTimeSeconds) {
	/* Player is dying, camera stops following. dying boolean will be set to
	false the same time dead boolean will be set to true. */
	if (target.dying) return;

	/* level->update will detect the player is dead because it gets called
	first and put him back on the platform, allowing us to do the necessary
	calculations for the camera. We are using target position because we want
	the following if to be executed only once. */
	if (target.pos.z < Constants::playerDeathBelowY) {
		// Reset camera and fieldOfView
		fieldOfView = Constants::initialFieldOfView;
		free = false;
		thirdPerson = true;
		animationOffset = Constants::cameraThirdPersonOffset;


		set(Constants::cameraThirdPersonPos,
			// Will look above the player.
			target.pos + glm::vec3(0, target.size.y, 0),
			glm::vec3(0, 1, 0));


		float distY = position.y - target.pos.y;
		float distZ = position.z - target.pos.z
			+ target.size.z / 2;
		angleYZDegrees = glm::degrees(glm::atan(distY / distZ));


		projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 400.0f);
	}
	// ------------------------------------------------------------------------


	/* If the camera is supposed to be free camera we won't allow it to stick
	to the target so we will have the following line where we just return from
	function. */
	if (free) return;
	// ------------------------------------------------------------------------

	// Update camera to have it stick to player.
	position = target.pos;

	glm::vec3 cameraOffset{  };
	glm::vec3 cameraTmpPos{ animationOffset };
	float sensitivity = 5;

	if (thirdPerson) {
		cameraOffset = Constants::cameraThirdPersonOffset;
	}
	else {
		cameraOffset = Constants::cameraFirstPersonOffset;
	}

	if (cameraTmpPos.x < cameraOffset.x) {
		cameraTmpPos.x += deltaTimeSeconds * sensitivity;
		animationOffset.x = min(cameraTmpPos.x, cameraOffset.x);
	}
	else if (cameraTmpPos.x > cameraOffset.x) {
		cameraTmpPos.x -= deltaTimeSeconds * sensitivity;
		animationOffset.x = max(cameraTmpPos.x, cameraOffset.x);
	}

	float vy = glm::sin(glm::radians(angleYZDegrees));
	float vz = glm::cos(glm::radians(angleYZDegrees));

	if (cameraTmpPos.y < cameraOffset.y) {
		cameraTmpPos.y += deltaTimeSeconds * sensitivity * vy;
		animationOffset.y = min(cameraTmpPos.y, cameraOffset.y);
	}
	else if (cameraTmpPos.y > cameraOffset.y) {
		cameraTmpPos.y -= deltaTimeSeconds * sensitivity * vy;
		animationOffset.y = max(cameraTmpPos.y, cameraOffset.y);
	}

	if (cameraTmpPos.z < cameraOffset.z) {
		cameraTmpPos.z += deltaTimeSeconds * sensitivity * vz;
		animationOffset.z = min(cameraTmpPos.z, cameraOffset.z);
	}
	else if (cameraTmpPos.z > cameraOffset.z) {
		cameraTmpPos.z -= deltaTimeSeconds * sensitivity * vz;
		animationOffset.z = max(cameraTmpPos.z, cameraOffset.z);
	}
	position += animationOffset;

	/* Camera position modifies on OZ axis depending on target speed in third
	person. */
	if (thirdPerson) {
		position.z += target.speed.z / 10;
	}
	// ------------------------------------------------------------------------
}
