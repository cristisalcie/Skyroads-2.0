#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <skyroads/objects3D/Object3D.h>
#include <skyroads/objects3D/LevelObject3D.h>
#include <skyroads/objects3D/Player3D.h>
#include <skyroads/objects3D/Platform3D.h>
#include <skyroads/objects3D/Asteroid3D.h>
#include <skyroads/common/Constants.h>
#include <skyroads/common/Collision3D.h>

class Level final {
protected:
    enum Chance {
        color_red,    // assigned 0 ; Instant death
        color_yellow, // assigned 1 ; Fuel penalty
        color_orange, // assigned 2 ; Speed
        color_green,  // assigned 3 ; Bonus fuel
        color_blue,   // assigned 4 ; Neutral
        color_max     // assigned 5 ; Max colors
    };
public:
    Level(FILE *logFile);
    ~Level();

    void initObjects();
    unsigned int generateNewColumn(LevelObject3D* lastSpawned,
        const unsigned int respawnObjNum, bool* occupiedColumn,
        const unsigned int maxColumns);

    void recalculatePlatform(
        glm::vec3& color,
        const unsigned int respawnObjNum,
        const float newSizeZ,
        bool& generatedRed,
        bool& generatedOrange,
        bool& generatedYellow,
        bool& generatedGreen);

    void regeneratePlatforms(const int rNum);

    void regenerateAsteroids(const int rNum);

    void update(float deltaTimeSeconds);

public:
    FILE* logFile;

    Player3D* player;
    /* Current existing level objects:
     *   Platform3D
     */
    std::vector<Platform3D*> platforms;
    std::vector<Asteroid3D*> asteroids;

    unsigned int lightObjects;

    LevelObject3D* lastSpawnedPlatform;
    LevelObject3D* lastSpawnedAsteroid;

    // Used to start the game. After death as well.
    bool started;

    // Used to generate platform colors.
    int chances[Chance::color_max];
};
