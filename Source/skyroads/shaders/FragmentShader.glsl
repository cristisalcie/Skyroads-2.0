#version 330

#define maxLights 50

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Uniform properties
uniform int num_lights;
uniform bool spotlight[maxLights];
uniform vec3 light_color[maxLights];
uniform vec3 light_position[maxLights];
uniform vec3 light_direction[maxLights];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform sampler2D texture_1;
uniform float elapsed_time;
uniform bool has_texture;
uniform bool is_player;
uniform bool is_background;
uniform bool lights_effects;

uniform vec3 object_color;
uniform bool has_color;

layout(location = 0) out vec4 out_color;

void main() {
	if (!lights_effects) { // Just give the color.
		vec4 color;
		float intensity = 1;
		vec2 new_texcoord = texcoord;
		if (has_texture) {
			if (is_background) {
				intensity = 0.6f;
				new_texcoord += mod(elapsed_time / 800, 100);
			}
			color = texture2D(texture_1, new_texcoord);
			if (has_color) {
				color = mix(color, vec4(object_color, 1), 0.5f);
			}
		} else {
			color = vec4(object_color, 1);
		}

		if (color.a < 0.5f) discard;
		out_color = color * intensity;
		return;
	}

	vec3 world_position = worldPosition;
	vec3 world_normal	= normalize(worldNormal);
	// Lightning color calculations.
	vec4 lights_out_color = vec4(0);
	for (int i = 0; i < num_lights && i < maxLights && !is_background; ++i) {
	
 		vec3 L = normalize(light_position[i] - world_position);
		vec3 V = normalize(eye_position - world_position);
		vec3 H = normalize(L + V) ;
		vec3 R = normalize(reflect(-L, world_normal));

		// Define ambient light component.
		float ambient_light = 0.001f;

		// Compute diffuse light component.
		float light_intensity = 1000;
		if (spotlight[i]) {
			light_intensity = 3;
		}
		float diffuse_light = material_kd * light_intensity
			* max(dot(world_normal, L), 0);

		// Compute specular light component.
		float receiveLight = 0.0f;
		if (diffuse_light > 0) {
			receiveLight = 1.0f;
		}


		vec3 specular_light;
		// Median variant.
		specular_light = material_ks * light_intensity * receiveLight * light_color[i]
			* pow(max(dot(world_normal, H), 0), material_shininess);

		// Compute light.
		float d = distance(light_position[i], world_position);
		float attenuationFactor = 1.0 / (1.2f + 11.1f * d + pow(d, 2));

		if (spotlight[i]) {
			float cut_off = radians(30);
			float spot_light = dot(-L, light_direction[i]);
			float spot_light_limit = cos(cut_off);

			if (spot_light > spot_light_limit) {
				float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
				float light_att_factor = pow(linear_att, 2);
				attenuationFactor = light_att_factor;
			}

			else attenuationFactor = 0;
		}

		vec3 computed_light = (ambient_light + attenuationFactor
			* (diffuse_light + specular_light));
		// Write pixel lights out color.
		lights_out_color += vec4(computed_light * light_color[i], 1);
	}
	// ------------------------------------------------------------------------
	if (has_color) {
		lights_out_color = mix(
			lights_out_color,
			vec4(object_color, 1.0f),
			0.5f);
	}

	vec4 color;
	// Texture color mix.
	if (has_texture) {
		vec4 texture_out_color = texture2D(texture_1, texcoord);

		float blendFactor = 0.25f;
		if (is_player) {
			blendFactor = 0.65f;
		}
		color = mix(lights_out_color, texture_out_color, blendFactor);
	} else {
		color = lights_out_color;	
	}

	if (color.a < 0.5f) discard;
	out_color = color;
}
