#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

#include <skyroads/objects3D/LevelObject3D.h>

class Platform3D : public LevelObject3D {
public:
    Platform3D( FILE* logFile,
                glm::vec3 pos = glm::vec3(0),
                glm::vec3 size = glm::vec3(1, 1, 1),
                glm::vec3 speed = glm::vec3(
                    0, 0, Constants::platformInitialSpeed),
                glm::vec3 color = Constants::simplePlatformColor);

    Platform3D( FILE* logFile,
                unsigned int column,
                float playerSizeY,
                glm::vec3 size = glm::vec3(1, 1, 1),
                glm::vec3 speed = glm::vec3(
                    0, 0, Constants::platformInitialSpeed),
                glm::vec3 color = Constants::simplePlatformColor);

    virtual ~Platform3D();

    void respawn(glm::vec3 pos) override;
    void respawn(unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color) override;
    void respawn(unsigned int line, unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color) override;
    bool hitObject(Player3D& player) override;
    bool hitObject(Platform3D& platform) override;
    bool hitByObject(Object3D& object) override;
    void update(float deltaTimeSeconds) override;
};
