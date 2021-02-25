#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <skyroads/objects3D/Object3D.h>

class LevelObject3D : public Object3D{
public:
    LevelObject3D();
    virtual ~LevelObject3D();

    // Updates visible boolean based on position/size.
    virtual void checkVisible();

    // Updates expired boolean based on position/size.
    virtual void checkExpired();

    // Respawn at a given column (coming towards player on Oz axis).
    virtual void respawn(unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color = glm::vec3(0, 0, 0)) = 0;

    /*
    Respawn at a given column and line.
        - column is coming towards player (Oz axis)
        - line is on Oy axis
    */
    virtual void respawn(unsigned int line, unsigned int column, glm::vec3 size, glm::vec3 speed, glm::vec3 color = glm::vec3(0, 0, 0)) = 0;

    // If object is visible, render it.
    bool visible;

    // If object is expired, reset it or queue to reset.
    bool expired;

    // Object's current line. Starts from 1.
    unsigned int line;

    // Object's current column. Starts from 1.
    unsigned int column;
};
