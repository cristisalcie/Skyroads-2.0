#version 330
precision mediump float;

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float elapsed_time;
uniform bool is_player;
uniform bool must_deform;

// Output values to fragment shader
out vec2 texcoord;
out vec3 worldPosition;
out vec3 worldNormal;

void main() {
	texcoord		= v_texture_coord;
	worldPosition = (Model * vec4(v_position, 1)).xyz;
	worldNormal	= normalize(mat3(Model) * v_normal);

	vec3 deformValue = vec3(
		abs(sin(elapsed_time * 1.4) + 1) / 3,
		abs(sin(elapsed_time * 0.3) * sin(elapsed_time * 0.1f)) / 4,
		abs(sin(elapsed_time * 0.72)) / 4 - 0.1) / 2;
	
	vec3 new_v_position = v_position;
	if (must_deform) {
		new_v_position *= 0.6f;			// Scale
		new_v_position *= 2 + v_normal; // Deformation
		new_v_position += -0.3f;		// Position
		new_v_position += deformValue;	// Movement
	}
	gl_Position = Projection * View * Model * vec4(new_v_position, 1);
}
