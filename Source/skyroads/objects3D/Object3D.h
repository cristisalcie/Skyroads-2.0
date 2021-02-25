#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <skyroads/common/Collision3D.h>
#include <skyroads/common/Constants.h>

class Player3D;
class Platform3D;

class Object3D {
public:
    Object3D();
    virtual ~Object3D();

    // = 0 means it is pure virtual function.
    void virtual update(float deltaTimeSeconds) = 0;

    /* The idea here is: If player hit platform then the platform is suffering.
    If we implement the platform hit player then the player is suffering. */
    virtual bool hitObject(Player3D& player) = 0;
    virtual bool hitObject(Platform3D& platform) = 0;
    virtual bool hitByObject(Object3D& object) = 0;

    virtual void respawn(glm::vec3 pos) = 0;

    FILE* logFile;

    glm::vec3 pos;
    glm::vec3 size;
    glm::vec3 color;

    // For platforms, all of them will have the same speed at once.
    glm::vec3 speed;
    glm::vec3 initialSpeed;
};
