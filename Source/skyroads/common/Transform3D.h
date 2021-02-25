#pragma once

#include <include/glm.h>

namespace Transform3D {
	// Translate matrix
	inline glm::mat4 translate(glm::vec3 position) {
		return glm::transpose(
			glm::mat4 (
				1, 0, 0, position.x, 
				0, 1, 0, position.y, 
				0, 0, 1, position.z,
				0, 0, 0, 1
			)
		);
	}

	// Scale matrix
	inline glm::mat4 scale(glm::vec3 size) {
		return glm::mat4 (
				size.x, 0,		0,		0, 
				0,		size.y, 0,		0, 
				0,		0,		size.z, 0,
				0,		0,		0,		1
			);
	}

	// Rotate matrix relative to the OZ axis
	inline glm::mat4 rotateOZ(float radians) {
		float cosU = glm::cos(radians);
		float sinU = glm::sin(radians);
		return glm::transpose (
			glm::mat4 (
				cosU, -sinU,	  0,	  0, 
				sinU,  cosU,	  0,	  0, 
				0,	   0,         1,      0,
				0,     0,	      0,  	  1
			)
		);
	}

	// Rotate matrix relative to the OY axis
	inline glm::mat4 rotateOY(float radians) {
		float cosU = glm::cos(radians);
		float sinU = glm::sin(radians);
		return glm::transpose (
			glm::mat4 (
				 cosU,  0,	  sinU,	  0, 
				 0,     1,    0,	  0, 
				-sinU,  0,    cosU,   0,
				 0,     0,	  0,  	  1
			)
		);
	}

	// Rotate matrix relative to the OX axis
	inline glm::mat4 rotateOX(float radians) {
		float cosU = glm::cos(radians);
		float sinU = glm::sin(radians);
		return glm::transpose (
			glm::mat4 (
				1, 0,	  0,	  0, 
				0, cosU, -sinU,	  0, 
				0, sinU,  cosU,   0,
				0, 0,	  0,  	  1
			)
		);
	}
}
