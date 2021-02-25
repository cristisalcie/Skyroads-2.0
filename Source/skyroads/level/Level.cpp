#include <skyroads/level/Level.h>
#include <Core/Engine.h>

Level::Level(FILE* logFile) {
    this->logFile = logFile;
    player = new Player3D(  logFile,
                            Constants::playerInitialPos,
                            Constants::playerInitialSize);

    initObjects();

    started = false;
    lightObjects = player->orbits.amount;

    for (int i = 0; i < Chance::color_max; ++i) {
        chances[i] = 0;
    }

    fprintf(logFile, "Created level\n");
}

Level::~Level() {
    delete player;
    for (Platform3D* obj : platforms) {
        delete obj;
    }
    for (Asteroid3D* obj : asteroids) {
        delete obj;
    }
    platforms.clear();
    asteroids.clear();
    fprintf(logFile, "Deleted level\n");
}

void Level::initObjects() {
    if (!platforms.empty()) {
        for (Platform3D* object : platforms) {
            delete object;
        }
        platforms.clear();
    }
    if (!asteroids.empty()) {
        for (Asteroid3D* object : asteroids) {
            delete object;
        }
        asteroids.clear();
    }

    Platform3D* initialPlatform = new Platform3D(
        logFile,
        player->pos,
        glm::vec3(Constants::startingPlatformSideX,
            Constants::startingPlatformHeight,
            Constants::startingPlatformSideZ)
    );
    initialPlatform->pos.y -= initialPlatform->size.y / 2;
    initialPlatform->pos.y -= player->size.y / 2 + 0.1f;
    initialPlatform->visible = true;
    platforms.push_back(initialPlatform);

    int maxPlatforms = 3;
    float sizeSideZ = Constants::renderDistanceZ / maxPlatforms;
    glm::vec3 color{ Constants::orangePlatformColor };
    for (int i = 1; i <= maxPlatforms; ++i) {
        if (i == maxPlatforms) {
            color = Constants::greenPlatformColor;
        }
        Platform3D* continueInitialPlatform = new Platform3D(
            logFile,
            initialPlatform->pos,
            glm::vec3(Constants::platformMaxSideX,
                Constants::platformMaxHeightY,
                sizeSideZ
            ),
            glm::vec3(0, 0, Constants::platformInitialSpeed),
            color
        );
        continueInitialPlatform->pos.z -= continueInitialPlatform->size.z / 2;
        continueInitialPlatform->pos.z -= initialPlatform->size.z / 2;
        continueInitialPlatform->pos.z -= Constants::platformsGapZ;

        continueInitialPlatform->pos.z -= (i - 1) * Constants::platformsGapZ;
        continueInitialPlatform->pos.z -= (i - 1) * sizeSideZ;

        if (i < maxPlatforms) {
            continueInitialPlatform->pos.y += maxPlatforms - i;
        }

        continueInitialPlatform->visible = true;
        platforms.push_back(continueInitialPlatform);
        lastSpawnedPlatform = continueInitialPlatform;

        /* We will start spawning asteroids when we start the spawning of the
        random platforms. */
        lastSpawnedAsteroid = continueInitialPlatform;
    }
}


unsigned int Level::generateNewColumn(LevelObject3D* lastSpawned,
    const unsigned int respawnObjNum, bool* occupiedColumn, const unsigned int maxColumns) {

    unsigned int newColumn = 1 + rand() % maxColumns;

    if (respawnObjNum == 1) {
        // Make sure it's not too far.
        if (lastSpawned->column == 1
            && newColumn == 3) {
            --newColumn;
        } else if (lastSpawned->column == maxColumns
            && newColumn == 1) {
            ++newColumn;
        }
    } else {
        // Make sure it doesn't overwrite existing column.
        while (occupiedColumn[newColumn - 1]) {
            ++newColumn;
            if (newColumn > maxColumns) {
                newColumn = 1;
            }
        }
    }
    return newColumn;
}

void Level::recalculatePlatform(
    glm::vec3& color,
    const unsigned int respawnObjNum,
    const float newSizeZ,
    bool& generatedRed,
    bool& generatedOrange,
    bool& generatedYellow,
    bool& generatedGreen) {

    color = Constants::simplePlatformColor;

    // Modify chances
    if (player->fuelTank.fuel / player->fuelTank.capacity < 0.8) {
        chances[Chance::color_green] = min(100,
            chances[Chance::color_green] + 10);
    }
    else {
        chances[Chance::color_yellow] = min(100,
            chances[Chance::color_yellow] + 10);
    }

    int extractedChance = rand() % 100;

    // Priority ordering
    if (!generatedRed) {
        if (respawnObjNum == 3) {
            /* generatedRed makes sure we only spawn 1 orange
            on a line. */
            color = Constants::redPlatformColor;
        }
    }
    if (!generatedOrange
        && newSizeZ >= Constants::platformMaxSideZ - 10) {
        /* generatedOrange makes sure we only spawn 1 orange
        on a line. */
        color = Constants::orangePlatformColor;
    }
    if (!generatedYellow
        && extractedChance < chances[Chance::color_yellow]) {
        color = Constants::yellowPlatformColor;
    }
    if (!generatedGreen
        && extractedChance < chances[Chance::color_green]) {
        color = Constants::greenPlatformColor;
    }

    if (color == Constants::redPlatformColor) {
        /* Red is not generated by chance, but when there are
        3 available choices to jump on. */
        generatedRed = true;
    }
    else if (color == Constants::yellowPlatformColor) {
        chances[Chance::color_yellow] = max(0,
            chances[Chance::color_yellow] - 40);
        generatedYellow = true;
    }
    else if (color == Constants::orangePlatformColor) {
        /* Orange is not generated by chance, but by platform
        length. */
        generatedOrange = true;
    }
    else if (color == Constants::greenPlatformColor) {
        chances[Chance::color_green] = max(0,
            chances[Chance::color_green] - 40);
        generatedGreen = true;
    }
    else {
        /* Nothing to do for now. Blue is generated in absence
        of another color. */
    }
}


void Level::regeneratePlatforms(const int rNum) {
    // Choose a random amount of active columns.
    unsigned int respawnObjNum = 1 + rNum % Constants::platformMaxColumns;

    int precision = 100;
    glm::vec3 newSize{};
    newSize.x = (Constants::platformMinSideX * precision
        + rNum % (int)
        ((Constants::platformMaxSideX - Constants::platformMinSideX)
            * precision))
        / precision;
    newSize.y = (Constants::platformMinHeightY * precision
        + rNum % (int)
        ((Constants::platformMaxHeightY - Constants::platformMinHeightY)
            * precision))
        / precision;
    newSize.z = (Constants::platformMinSideZ * precision
        + rNum % (int)
        ((Constants::platformMaxSideZ - Constants::platformMinSideZ)
            * precision))
        / precision;

    glm::vec3 newSpeed{ glm::vec3(0) };
    newSpeed.z = Constants::platformInitialSpeed;

    int remainingToSpawn = respawnObjNum;

    bool occupiedColumn[Constants::platformMaxColumns]{}; // Default is false.

    glm::vec3 color{};
    bool generatedRed = false;
    bool generatedOrange = false;
    bool generatedYellow = false;
    bool generatedGreen = false;

    // We must generate a platform.
    for (Platform3D* object : platforms) {
        if (object->expired) {
            unsigned int newColumn = generateNewColumn(lastSpawnedPlatform,
                respawnObjNum, occupiedColumn, Constants::platformMaxColumns);

            recalculatePlatform(color, respawnObjNum, newSize.z, generatedRed, generatedOrange, generatedYellow, generatedGreen);

            object->respawn(newColumn, newSize, newSpeed, color);
            occupiedColumn[newColumn - 1] = true;
            --remainingToSpawn;
            lastSpawnedPlatform = object;
        }
        if (remainingToSpawn <= 0) break;
    }

    while (remainingToSpawn > 0) {
        // There were no expired platforms. We must make new ones.
        unsigned int newColumn = generateNewColumn(lastSpawnedPlatform,
            respawnObjNum, occupiedColumn, Constants::platformMaxColumns);

        recalculatePlatform(color, respawnObjNum, newSize.z, generatedRed, generatedOrange, generatedYellow, generatedGreen);

        Platform3D* object = new Platform3D(logFile, newColumn,
            player->size.y, newSize, newSpeed, color);
        platforms.push_back(object);

        occupiedColumn[newColumn - 1] = true;
        --remainingToSpawn;
        lastSpawnedPlatform = object;
    }
}

void Level::regenerateAsteroids(const int rNum) {
    for (int line = 1; line <= Constants::asteroidMaxLines; ++line) {
        // Choose a random amount of active columns. Can be 0.
        unsigned int respawnObjNum = (rNum + line) % (Constants::asteroidMaxColumns / 3);

        glm::vec3 newSpeed{};
        newSpeed.z = Constants::asteroidsInitialSpeed;

        int remainingToSpawn = respawnObjNum;

        bool occupiedColumn[Constants::asteroidMaxColumns]{}; // Default is false.

        // We must generate an asteroid.

        for (Asteroid3D* object : asteroids) {
            if (object->expired) {
                unsigned int newColumn = generateNewColumn(lastSpawnedAsteroid,
                    respawnObjNum, occupiedColumn, Constants::asteroidMaxColumns);

                /* Size at respawn doesn't matter here because it's recalculated
                for each individual asteroid. */
                if (!object->mustDeform && lightObjects > 0) {
                    --lightObjects;
                }
                object->respawn(line, newColumn, glm::vec3(0), newSpeed);
                if (!object->mustDeform) {
                    ++lightObjects;
                }

                occupiedColumn[newColumn - 1] = true;
                --remainingToSpawn;
                lastSpawnedAsteroid = object;
            }
            if (remainingToSpawn <= 0) break;
        }

        while (remainingToSpawn > 0) {
            // There were no expired asteroids. We must make new ones.
            unsigned int newColumn = generateNewColumn(lastSpawnedAsteroid,
                respawnObjNum, occupiedColumn, Constants::asteroidMaxColumns);

            /* Size at respawn doesn't matter here because it's recalculated
            for each individual asteroid. */
            Asteroid3D* object = new Asteroid3D(logFile, line, newColumn,
                glm::vec3(0), newSpeed);
            asteroids.push_back(object);
            if (!object->mustDeform) {
                ++lightObjects;
            }

            occupiedColumn[newColumn - 1] = true;
            --remainingToSpawn;
            lastSpawnedAsteroid = object;
        }
    }
}

void Level::update(float deltaTimeSeconds) {
    if (!started) return;

    // Handle death of player.
    {
        if (player->dying) {
            bool updatedPlayerSpeed = false;
            for (Platform3D* object : platforms) {
                if (!updatedPlayerSpeed && !object->expired
                    && object->visible) {
                    /* Faster than the objects were moving previously, so we
                    can actually catch that the ball is moving on z axis. */
                    player->speed.z += object->speed.z * 2;
                    updatedPlayerSpeed = true;
                }
                object->speed.z = 0.0f;
            }
        } else if (player->dead) {
            initObjects();   // Resets all platforms.
            started = false;
            return;
        }
    }
    // ------------------------------------------------------------------------


    // Generate/Regenerate platforms and asteroids.
    {
        float lastSpawnedPlatformFrontZ{
            lastSpawnedPlatform->pos.z - lastSpawnedPlatform->size.z / 2
        };

        float lastSpawnedAsteroidFrontZ{
            lastSpawnedAsteroid->pos.z - lastSpawnedAsteroid->size.z / 2
        };

        // Random number.
        unsigned int rNum = rand();

        // Generate/Regenerate platforms if necesarry.
        if (lastSpawnedPlatformFrontZ > -Constants::renderDistanceZ
            + Constants::platformsGapZ) {
            regeneratePlatforms(rNum);
        }

        // Generate/Regenerate asteroids if necesarry.
        if (lastSpawnedAsteroidFrontZ > -Constants::renderDistanceZ
            + Constants::asteroidsGap.z) {
            regenerateAsteroids(rNum);
        }
    }
    // ------------------------------------------------------------------------


    // Update all objects3D
    {
        for (Platform3D* object : platforms) {
            object->update(deltaTimeSeconds);
            if (!player->dying) {
                /* We must recalculate these here because object doesn't have
                access to player and vice versa. */
                object->speed.z = object->initialSpeed.z + player->speed.z;
                player->rotationSpeed = object->speed.z;
            }
        }
        for (Asteroid3D* object : asteroids) {
            object->update(deltaTimeSeconds);
            if (!player->dying) {
                /* We must recalculate these here because object doesn't have
                access to player and vice versa. */
                object->speed.z = object->initialSpeed.z + player->speed.z;
            }
        }
        player->update(deltaTimeSeconds);
    }
    // ------------------------------------------------------------------------


    // Platform collisions
    {
        for (Platform3D* object : platforms) {
            if (object->expired || !object->visible) continue; 
            /* Order here matters !!! Player must be affected first, by
            convention. */
            player->hitByObject(*object);
            object->hitByObject(*player);
        }
        if (player->collidedFront && !player->onPlatform) {
            // Stop all platforms
            for (Platform3D* object : platforms) {
                object->speed.z = 0.0f;
            }
            player->speed.z = 0.0f; // Because it hit something frontally.
        }
    }
    // ------------------------------------------------------------------------
}
