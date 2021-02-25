#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <skyroads/objects3D/LevelObject3D.h>

class Asteroid3D : public LevelObject3D {
public:
    Asteroid3D( FILE* logFile,
                glm::vec3 pos = glm::vec3(0),
                glm::vec3 size = glm::vec3(1, 1, 1),
                glm::vec3 speed = glm::vec3(
                    0, 0, Constants::platformInitialSpeed),
                glm::vec3 color = glm::vec3(0.4f));

    Asteroid3D( FILE* logFile,
                unsigned int line,
                unsigned int column,
                glm::vec3 size = glm::vec3(1, 1, 1),
                glm::vec3 speed = glm::vec3(
                    0, 0, Constants::platformInitialSpeed),
                glm::vec3 color = glm::vec3(0.4f));

    bool mustDeform;
    // Used to have a varying rotation on OY axis only. Gets updated in Level.cpp
    float rotationSpeed;
    float angle; // To rotate with rotationSpeed.

    virtual ~Asteroid3D();

    void respawn(glm::vec3 pos) override;
    void respawn(unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color = glm::vec3(0, 0, 0)) override;
    void respawn(unsigned int line, unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color = glm::vec3(0, 0, 0)) override;
    bool hitObject(Player3D& player) override;
    bool hitObject(Platform3D& platform) override;
    bool hitByObject(Object3D& object) override;
    void update(float deltaTimeSeconds) override;

    // Updates visible boolean based on position/size.
    void checkVisible() override;
    // Updates expired boolean based on position/size.
    void checkExpired() override;
};
