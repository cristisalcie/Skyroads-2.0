#pragma once
#include <Component/SimpleScene.h>
#include <skyroads/common/Constants.h>
#include <skyroads/common/Transform3D.h>
#include <skyroads/common/GameCamera.h>
#include <skyroads/objects3D/Platform3D.h>
#include <skyroads/objects3D/Player3D.h>
#include <skyroads/level/Level.h>

class Skyroads : public SimpleScene {
	public:
		Skyroads();
		~Skyroads();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;


		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;

		void OnMouseMove(int mouseX, int mouseY,
			int deltaX, int deltaY) override;

		void OnMouseBtnPress(int mouseX, int mouseY,
			int button, int mods) override;

		void OnMouseBtnRelease(int mouseX, int mouseY,
			int button, int mods) override;

		void OnMouseScroll(int mouseX, int mouseY,
			int offsetX, int offsetY) override;

		void OnWindowResize(int width, int height) override;

		void RenderMesh(Mesh* mesh, Shader* shader,
			const glm::mat4& modelMatrix) override;

		void RenderSimpleMesh(Mesh* mesh,
			Shader* shader,
			const glm::mat4& modelMatrix,
			const glm::vec3& color,
			bool hasColor = true,
			bool lightsEffects = true,
			Texture2D* texture1 = nullptr,
			bool must_deform = false,
			unsigned int numLights = 0,
			glm::vec3* light_color = nullptr,
			glm::vec3* light_position = nullptr,
			glm::vec3* light_direction = nullptr,
			bool* spotlight = nullptr,
			bool plotLights = false);

		void renderObjects3D();
		void renderAsteroid3D(Asteroid3D& asteroid);
		void renderPlatform3D(Platform3D& platform);
		void renderPlayer3D(Player3D& player);
		void renderFuel(Player3D& player);
		void renderBackground();

	protected:
		typedef struct {
			unsigned int shininess;
			float Kd;
			float Ks;
		} MaterialProperties;
		
		FILE* logFile;

		Game::Camera* camera;
		Level* level;
		bool paused;

		std::unordered_map<std::string, Texture2D*> mapTextures;
		MaterialProperties materialProp;
};
