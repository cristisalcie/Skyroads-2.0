#include <skyroads/Skyroads.h>

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

Skyroads::Skyroads() {
	logFile = nullptr;
	logFile = fopen("log.txt", "w");
	if (logFile == nullptr) {
		std::cout << "Error at oppening \"log.txt\"\n";
		Engine::Exit();
		std::exit(-1);
	}

	camera = new Game::Camera();
	level = new Level(logFile);


	// Light & material properties
	{
		materialProp.shininess = 30;
		materialProp.Kd = 0.5f;
		materialProp.Ks = 0.5f;
	}
	// ------------------------------------------------------------------------

	paused = false;

	fprintf(logFile, "Started game.\n");
}

Skyroads::~Skyroads() {
	delete camera;
	delete level;
	fprintf(logFile, "Closed game.\n");
	fclose(logFile);
}

void Skyroads::Init() {
	// Set camera.
	{
		camera->animationOffset = Constants::cameraThirdPersonOffset;
		camera->set(Constants::cameraThirdPersonPos,
					// Will look above the player.
					level->player->pos + glm::vec3(0, level->player->size.y, 0),
					glm::vec3(0, 1, 0));
		float distY = camera->position.y - level->player->pos.y;
		float distZ = camera->position.z - level->player->pos.z
			+ level->player->size.z / 2;
		camera->angleYZDegrees = glm::degrees(glm::atan(distY / distZ));

		camera->projectionMatrix = glm::perspective(
			RADIANS(camera->fieldOfView),
			window->props.aspectRatio,
			0.01f,
			400.0f);
	}
	// ------------------------------------------------------------------------


	// Load meshes
	{
		Mesh* mesh;
		mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;

		mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	// ------------------------------------------------------------------------


	// Load textures.
	{
		const std::string textureLoc = "Source/skyroads/Textures/";

		Texture2D* texture;
		texture = new Texture2D();
		texture->Load2D((textureLoc + "universe.png").c_str(), GL_REPEAT);
		mapTextures["universe"] = texture;

		texture = new Texture2D();
		texture->Load2D((textureLoc + "asteroid.jpg").c_str(), GL_REPEAT);
		mapTextures["player"] = texture;

		texture = new Texture2D();
		texture->Load2D((textureLoc + "steel.jpg").c_str(), GL_REPEAT);
		mapTextures["steel"] = texture;

		texture = new Texture2D();
		texture->Load2D((textureLoc + "asteroid.jpg").c_str(), GL_REPEAT);
		mapTextures["asteroid"] = texture;

		texture = new Texture2D();
		texture->Load2D((textureLoc + "star.jpg").c_str(), GL_REPEAT);
		mapTextures["star"] = texture;

		texture = new Texture2D();
		texture->Load2D((textureLoc + "fire.jpg").c_str(), GL_REPEAT);
		mapTextures["spotlight"] = texture;
	}
	// ------------------------------------------------------------------------


	// Create a shader program.
	{
		Shader* shader = new Shader("gameShader");
		shader->AddShader("Source/skyroads/shaders/VertexShader.glsl",
			GL_VERTEX_SHADER);
		shader->AddShader("Source/skyroads/shaders/FragmentShader.glsl",
			GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	// ------------------------------------------------------------------------


	// Print instructions.
	{
		std::cout << "Press ESC to quit the game.\n";
		std::cout << "Press W/A/S/D to start the game.\n";
		std::cout << "Press C to change camera.\n";
		std::cout << "Press SPACE to jump camera.\n";
		std::cout << "Press P to pause/unpause the game.\n";
	}
	// ------------------------------------------------------------------------
}

void Skyroads::FrameStart() {
	// Clear the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Skyroads::Update(float deltaTimeSeconds) {
	if (!paused) {
		level->update(deltaTimeSeconds);
	}
	camera->update(*level->player, window, deltaTimeSeconds);
	renderObjects3D();
	renderBackground();

}

void Skyroads::FrameEnd() {
	DrawCoordinatSystem(camera->getViewMatrix(), camera->projectionMatrix);
}

void Skyroads::OnInputUpdate(float deltaTime, int mods) {

	if (camera->free) {
		float sensitivity = 5;
		if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
			sensitivity = 25;
		}

		// Move camera.
		if (window->KeyHold(GLFW_KEY_W)) {
			// Translate the camera forward
			camera->moveForward(deltaTime * sensitivity);
		}
		if (window->KeyHold(GLFW_KEY_A)) {
			// Translate the camera to the left
			camera->translateRight(-deltaTime * sensitivity);
		}
		if (window->KeyHold(GLFW_KEY_S)) {
			// Translate the camera backwards
			camera->moveForward(-deltaTime * sensitivity);
		}
		if (window->KeyHold(GLFW_KEY_D)) {
			// Translate the camera to the right
			camera->translateRight(deltaTime * sensitivity);
		}
		if (window->KeyHold(GLFW_KEY_Q)) {
			// Translate the camera down
			camera->translateUpword(-deltaTime * sensitivity);
		}
		if (window->KeyHold(GLFW_KEY_E)) {
			// Translate the camera up
			camera->translateUpword(deltaTime * sensitivity);
		}
	}
	else if (!level->player->dead && !paused) {
		float sensitivityOx = 4;
		// User key bindings.
		if (window->KeyHold(GLFW_KEY_W) && !level->player->collidedFront
			&& !level->player->lockedSpeed && !level->player->dying) {

			// Increase player speed.
			float newSpeed = level->player->speed.z + deltaTime
				* Constants::playerReachMaxSpeedTime;
			level->player->speed.z = glm::min(
				newSpeed,
				Constants::playerMaxSpeed);

			// Increaase level objects (platforms) speed.
			for (Platform3D* object : level->platforms) {
				object->speed.z = object->initialSpeed.z
					+ level->player->speed.z;
			}
		}

		if (window->KeyHold(GLFW_KEY_A) && !level->player->dying) {
			// Translate the player to the left
			if (level->player->collidedLeft &&
				level->player->onPlatform) {
				// Hit corner simulate jump
				level->player->bounceBack =
					Constants::playerHitCornerJumpDistance;
				level->player->speed.y =
					-Constants::playerHitCornerJumpDistance;
				level->player->onPlatform = false;
			}
			if (!level->player->collidedLeft) {
				level->player->pos.x -= sensitivityOx * deltaTime;
			}
		}

		if (window->KeyHold(GLFW_KEY_S) && !level->player->collidedBack
			&& !level->player->lockedSpeed && !level->player->dying) {

			// Decrease player speed.
			float newSpeed = level->player->speed.z - deltaTime
				* Constants::playerReachMaxSpeedTime;
			level->player->speed.z = glm::max(0.0f, newSpeed);

			// Decrease level objects (platforms) speed.
			for (Platform3D* object : level->platforms) {
				object->speed.z = object->initialSpeed.z
					+ level->player->speed.z;
			}
		}

		if (window->KeyHold(GLFW_KEY_D) && !level->player->dying) {
			// Translate the player to the right
			if (level->player->collidedRight &&
				level->player->onPlatform) {
				// Hit corner simulate jump
				level->player->bounceBack =
					Constants::playerHitCornerJumpDistance;
				level->player->speed.y =
					-Constants::playerHitCornerJumpDistance;
				level->player->onPlatform = false;
			}
			if (!level->player->collidedRight) {
				level->player->pos.x += sensitivityOx * deltaTime;
			}
		}
		// Player jump.
		if (window->KeyHold(GLFW_KEY_SPACE)
			&& !level->player->fuelTank.empty
			&& !level->player->dying
			&& !level->player->collidedUp && level->player->onPlatform) {
			level->player->bounceBack = Constants::playerJumpDistance * deltaTime;
			level->player->speed.y = -Constants::playerJumpDistance * deltaTime;
			level->player->onPlatform = false;
		}
		// ------------------------------------------------------------------------
	}
}

void Skyroads::OnKeyPress(int key, int mods) {
	// Pause/Unpause game.
	if (key == GLFW_KEY_P && level->started) {
		paused = !paused;
		if (paused) {
			std::cout << "Game is paused.\n";
		} else {
			std::cout << "Game is unpaused.\n";
		}
	}
	// ------------------------------------------------------------------------


	// Start level for the first time.
	if (!level->started && !camera->free) {
		if (key == GLFW_KEY_W || key == GLFW_KEY_A
			|| key == GLFW_KEY_S || key == GLFW_KEY_D) {
			level->started = true;
			level->player->dead = false; // For the replay situation.
			std::cout << "\nPlatform color codes:\n";
			std::cout << "Blue platform is neutral.\n";
			std::cout << "Red platform kills you instantly.\n";
			std::cout << "Yellow platform consumes a part of your fuel.\n";
			std::cout << "Orange platform gets you stucked at insane speed for"
				<< " a few seconds.\n";
			std::cout << "Green platform grants you extra fuel.\n\n";
		}
	}
	// ------------------------------------------------------------------------


	// Free camera
	if (key == GLFW_KEY_ENTER) {
		if (camera->free) {
			/* Position matters in order to calculate where it looks at
			properly. Hence position is the one we started the game with. */
			camera->set(Constants::cameraThirdPersonOffset + level->player->pos,
				// Will look above the player.
				level->player->pos + glm::vec3(0, level->player->size.y, 0),
				glm::vec3(0, 1, 0));
		}
		camera->free = !camera->free;
	}
	// ------------------------------------------------------------------------


	// Switch camera
	if ((level->player->pos.y > Constants::playerDyingBelowY)
		&& key == GLFW_KEY_C && !camera->free) {
		camera->thirdPerson = !camera->thirdPerson;
	}
	// ------------------------------------------------------------------------
}

void Skyroads::OnKeyRelease(int key, int mods) {
	// add key release event
}

void Skyroads::OnMouseMove(int mouseX, int mouseY, int deltaX,
	int deltaY) {
	if (camera->free && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float sensivityOX = 0.002f;
		float sensivityOY = 0.002f;

		if (window->GetSpecialKeyState() == 0) {
			/* Rotate the camera in First-person mode around OX and OY using
			deltaX and deltaY. Use the sensitivity variables for setting up
			the rotation speed. */
			camera->rotateFirstPerson_OX(-sensivityOY * deltaY);
			camera->rotateFirstPerson_OY(-sensivityOX * deltaX);
		}
	}
}

void Skyroads::OnMouseBtnPress(int mouseX, int mouseY, int button,
	int mods) {
	// add mouse button press event
}

void Skyroads::OnMouseBtnRelease(int mouseX, int mouseY, int button,
	int mods) {
	// add mouse button release event
}

void Skyroads::OnMouseScroll(int mouseX, int mouseY, int offsetX,
	int offsetY) {
}

void Skyroads::OnWindowResize(int width, int height) {
	camera->projectionMatrix = glm::perspective(RADIANS(camera->fieldOfView),
		window->props.aspectRatio, 0.01f, 400.0f);
}

void Skyroads::RenderMesh(Mesh* mesh, Shader* shader,
	const glm::mat4& modelMatrix) {
	if (!mesh || !shader || !shader->program)
		return;

	// Render an object using the specified shader and the specified position.
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
		glm::value_ptr(camera->getViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
		glm::value_ptr(camera->projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
		glm::value_ptr(modelMatrix));

	mesh->Render();
}

void Skyroads::RenderSimpleMesh(Mesh* mesh, Shader* shader,
								const glm::mat4& modelMatrix,
								const glm::vec3& color,
								bool hasColor,
								bool noLightsEffects,
								Texture2D* texture1,
								bool must_deform,
								unsigned int numLights,
								glm::vec3* light_color,
								glm::vec3* light_position,
								glm::vec3* light_direction,
								bool* spotlight,
								bool plotLights) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position.
	glUseProgram(shader->program);

	GLint location;

	// Bind model matrix
	location = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(location, 1, GL_FALSE,
		glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->getViewMatrix();
	location = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(location, 1, GL_FALSE,
		glm::value_ptr(camera->getViewMatrix()));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	location = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(location, 1, GL_FALSE,
		glm::value_ptr(camera->projectionMatrix));

	// Send application time.
	location = glGetUniformLocation(shader->GetProgramID(), "elapsed_time");
	GLfloat elapsed_time{ static_cast<GLfloat>(Engine::GetElapsedTime()) };
	glUniform1f(location, elapsed_time);


	// To only texture objects that have a texture.
	bool hasTexture = true;
	if (texture1 == nullptr) {
		hasTexture = false;
	}
	glUniform1i(glGetUniformLocation(shader->program, "has_texture"),
		hasTexture);

	if (texture1) {
		// Activate texture.
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture ID.
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		// Send texture uniform value.
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
	}

	// Object color.
	location = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(location, color.r, color.g, color.b);

	glUniform1i(glGetUniformLocation(shader->program, "has_color"), hasColor);

	// ------------------------------------------------------------------------

	// Set light position uniforms.
	if (plotLights) {
		/* Only player will send lights data. Everyone will have them unless
		they get sent another ones. */

		glUniform1i(glGetUniformLocation(shader->program, "num_lights"), numLights);

		std::string inShaderSpotlight;
		std::string inShaderColorName;
		std::string inShaderPositionName;
		std::string inShaderDirectionName;
		for (int i = 0; i < numLights; ++i) {
			inShaderSpotlight = "spotlight[" + std::to_string(i) + ']';
			inShaderColorName = "light_color[" + std::to_string(i) + ']';
			inShaderPositionName = "light_position[" + std::to_string(i) + ']';
			inShaderDirectionName = "light_direction[" + std::to_string(i) + ']';

			location = glGetUniformLocation(shader->program, inShaderSpotlight.data());
			glUniform1i(location, spotlight[i]);

			location = glGetUniformLocation(shader->program, inShaderColorName.data());
			glUniform3f(location, light_color[i].r, light_color[i].g,
				light_color[i].b);

			location = glGetUniformLocation(shader->program, inShaderPositionName.data());
			glUniform3f(location, light_position[i].x, light_position[i].y,
				light_position[i].z);

			location = glGetUniformLocation(shader->program, inShaderDirectionName.data());
			glUniform3f(location, light_direction[i].x, light_direction[i].y,
				light_direction[i].z);
		}
	}
	// ------------------------------------------------------------------------


	// Set eye position (camera position) uniform.
	glm::vec3 eyePosition = camera->position;
	location = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(location, eyePosition.x, eyePosition.y, eyePosition.z);


	// Set material property uniforms (shininess, kd, ks) 
	location = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(location, materialProp.shininess);

	location = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(location, materialProp.Kd);

	location = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(location, materialProp.Ks);
	// ------------------------------------------------------------------------


	glUniform1f(location, materialProp.Ks);

	// Send is_player boolean. Only player has texture "player".
	bool is_player = false;
	if (texture1 != nullptr
		&& texture1 == mapTextures["player"]) {
		is_player = true;
	}
	glUniform1i(glGetUniformLocation(shader->program, "is_player"), is_player);

	// Send is_background boolean
	bool is_background = false;
	if (texture1 != nullptr
		&& (texture1 == mapTextures["universe"])) {
		is_background = true;
	}
	glUniform1i(glGetUniformLocation(shader->program, "is_background"),
		is_background);

	// Send noLightsEffect boolean
	glUniform1i(glGetUniformLocation(shader->program, "lights_effects"),
		noLightsEffects);

	// Send must_deform boolean
	glUniform1i(glGetUniformLocation(shader->program, "must_deform"),
		must_deform);
	// ------------------------------------------------------------------------


	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()),
		GL_UNSIGNED_SHORT, 0);
}

void Skyroads::renderObjects3D() {
	for (Platform3D* platform : level->platforms) {
		renderPlatform3D(*platform);
	}
	for (Asteroid3D* asteroid : level->asteroids) {
		renderAsteroid3D(*asteroid);
	}
	renderPlayer3D(*(level->player));
}

void Skyroads::renderAsteroid3D(Asteroid3D& asteroid) {
	if (!asteroid.visible) return;

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::translate(asteroid.pos);
	modelMatrix *= Transform3D::scale(asteroid.size);
	modelMatrix *= Transform3D::rotateOX(glm::radians(asteroid.angle));



	std::string texName{ "star" };
	bool lightsEffect = false;
	if (asteroid.mustDeform) {
		texName = "asteroid";
		lightsEffect = true;
	}
	RenderSimpleMesh(meshes["sphere"], shaders["gameShader"], modelMatrix,
		asteroid.color, false, lightsEffect, mapTextures[texName], asteroid.mustDeform);
}

void Skyroads::renderPlatform3D(Platform3D& platform) {
	if (!platform.visible) return;

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::translate(platform.pos);
	modelMatrix *= Transform3D::scale(platform.size);

	RenderSimpleMesh(meshes["box"], shaders["gameShader"], modelMatrix,
		platform.color, true, true, mapTextures["steel"]);
}

void Skyroads::renderPlayer3D(Player3D& player) {
	// Render player fuel.
	renderFuel(player);

	glm::mat4 modelMatrix;
	if (camera->thirdPerson) {
		// Render player.
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::translate(player.pos);
		modelMatrix *= Transform3D::scale(player.size);
		modelMatrix *= Transform3D::rotateOX(glm::radians(player.angle));
		RenderSimpleMesh(meshes["sphere"], shaders["gameShader"], modelMatrix,
			player.color, false, true, mapTextures["player"], player.mustDeform);
	}

	// Orbits
	float elapsedTime = Engine::GetElapsedTime();
	float rotationSpeed = player.orbits.rotationSpeed;
	float radius = player.orbits.radius;

	glm::vec3* orbitPos = new glm::vec3[player.orbits.amount];
	glm::vec3* light_directions = new glm::vec3[level->lightObjects];
	glm::vec3* light_positions = new glm::vec3[level->lightObjects];
	glm::vec3* light_colors = new glm::vec3[level->lightObjects];
	bool* spotlight = new bool[level->lightObjects];

	int i;
	for (i = 0; i < player.orbits.amount; ++i) {
		float angle = i * 360 / player.orbits.amount;
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::translate(player.pos);
		modelMatrix *= Transform3D::translate(glm::vec3(
			glm::sin(glm::radians(angle) + rotationSpeed * elapsedTime) * radius,
			player.size.y,
			glm::cos(glm::radians(angle) + rotationSpeed * elapsedTime) * radius));
		modelMatrix *= Transform3D::scale(player.orbits.scale);
		modelMatrix *= Transform3D::rotateOY(glm::radians(player.orbits.angle));

		// Just render orbits at this stage.
		RenderSimpleMesh(meshes["sphere"], shaders["gameShader"], modelMatrix,
			glm::vec3(0), false, false, mapTextures["spotlight"]);

		// Set light positions to be rendered.
		light_positions[i] = player.pos + glm::vec3(
			glm::sin(glm::radians(angle) + rotationSpeed * elapsedTime) * radius,
			player.size.y,
			glm::cos(glm::radians(angle) + rotationSpeed * elapsedTime) * radius);
		light_directions[i] = glm::vec3(0, -1, 0);
		light_colors[i] = glm::vec3(1.0f, 1.0f, 0.0f);
		spotlight[i] = true;
	}

	for (Asteroid3D* object : level->asteroids) {
		if (!object->mustDeform) {
			// Optional: light directions / colors
			light_positions[i] = object->pos;
			light_directions[i] = glm::vec3(0, -1, 0);
			light_colors[i] = glm::vec3(0.4f, 0.4f, 1.0f);
			spotlight[i] = false;

			++i;
		}
	}

	modelMatrix = glm::mat3(0);
	// Plot all lights
	RenderSimpleMesh(meshes["sphere"], shaders["gameShader"], modelMatrix,
		glm::vec3(0), false, true, nullptr, false, level->lightObjects,
		light_colors, light_positions, light_directions, spotlight, true);

	delete[] spotlight;
	delete[] light_colors;
	delete[] light_positions;
	delete[] light_directions;
	delete[] orbitPos;
	// ------------------------------------------------------------------------
}

void Skyroads::renderFuel(Player3D& player) {
	glm::mat4 modelMatrix = glm::mat4(1);

	glm::vec3 size{ glm::vec3(
		player.fuelTank.fuel / 20,
		0.01f,
		0.01f) };

	glm::vec3 position{ glm::vec3(
		camera->position.x - player.fuelTank.capacity / 40,
		camera->position.y + 0.725f,
		camera->position.z - 2) };

	modelMatrix *= Transform3D::translate(position);
	// 0.5f because the box mesh side is 1.
	modelMatrix *= Transform3D::scale(size);
	modelMatrix *= Transform3D::translate(glm::vec3(0.5f, 0.5f, 0));

	RenderSimpleMesh(meshes["box"], shaders["gameShader"], modelMatrix,
		glm::vec3(1), true, false);

	// Full bar size render.
	size.x = player.fuelTank.capacity / 20;

	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::translate(position);
	// 0.5f because the box mesh side is 1.
	modelMatrix *= Transform3D::scale(size);
	modelMatrix *= Transform3D::translate(glm::vec3(0.5f, 0.5f, 0));

	RenderSimpleMesh(meshes["box"], shaders["gameShader"], modelMatrix,
		glm::vec3(0.5f), true, false);
}

void Skyroads::renderBackground() {
	glm::vec3 color{ glm::vec3(0) };

	glm::mat4 modelMatrix = glm::mat4(1);

	modelMatrix *= Transform3D::scale(glm::vec3(310));
	modelMatrix *= Transform3D::rotateOZ(glm::radians(90.0f));
	modelMatrix *= Transform3D::rotateOY(glm::radians(90.0f));

	RenderSimpleMesh(meshes["sphere"], shaders["gameShader"], modelMatrix,
		color, false, false, mapTextures["universe"]);
}
