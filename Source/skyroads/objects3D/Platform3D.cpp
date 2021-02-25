#include <skyroads/objects3D/Platform3D.h>
#include <skyroads/objects3D/Player3D.h>

Platform3D::Platform3D( FILE* logFile,
                        glm::vec3 pos,
                        glm::vec3 size,
                        glm::vec3 speed,
                        glm::vec3 color) {
    this->logFile = logFile;
    fprintf(logFile, "Created platform\n");
    this->pos = pos;
    this->size = size;
    this->speed = speed;
    initialSpeed = speed;
    visible = false;
    expired = false;
    this->color = color;
}

Platform3D::Platform3D( FILE* logFile,
                        unsigned int column,
                        float playerSizeY,
                        glm::vec3 size,
                        glm::vec3 speed,
                        glm::vec3 color) {
    this->logFile = logFile;
    fprintf(logFile, "Created platform\n");
    this->color = color;
    this->column = column;

    this->size = size;
    // Individual size on Ox on a specific column
    int precision = 10000;
    this->size.x = (Constants::platformMinSideX * precision
        + rand() % (int)(Constants::platformMaxSideX * precision))
        / precision;

    this->speed = speed;

    pos.x = Constants::platformMinSpawnX + (column - 1) * Constants::platformsGapX;

    // Below player.
    pos.y = Constants::playerInitialPos.y - this->size.y / 2 - playerSizeY / 2;

    pos.z = -Constants::renderDistanceZ - this->size.z / 2;

    initialSpeed = speed;
    visible = false;
    expired = false;
}

Platform3D::~Platform3D() {
    fprintf(logFile, "Deleted platform\n");
}

void Platform3D::respawn(glm::vec3 pos) {
}

void Platform3D::respawn(unsigned int column, glm::vec3 size, glm::vec3 speed,
    glm::vec3 color) {
    this->color = color;
    this->column = column;
    this->size = size;

    // Individual size on Ox on a specific column
    int precision = 10000;
    this->size.x = (Constants::platformMinSideX * precision
        + rand() % (int)(Constants::platformMaxSideX * precision))
        / precision;

    this->speed = speed;
    pos.x = Constants::platformMinSpawnX + (column - 1) * Constants::platformsGapX;
    pos.z = -Constants::renderDistanceZ - this->size.z / 2;
    visible = false;
    expired = false;
}

void Platform3D::respawn(unsigned int line, unsigned int column,
    glm::vec3 size, glm::vec3 speed, glm::vec3 color) {
}

bool Platform3D::hitObject(Player3D& player) {
    // Note!! This runs before player hit's platform.
    glm::vec3 minPos{ pos };
    glm::vec3 maxPos{ pos };

    float sectionPlatform = size.y / 40;
    minPos.x -= size.x / 2;
    minPos.y -= size.y / 2;
    minPos.z -= size.z / 2;

    maxPos.x += size.x / 2;
    maxPos.y += size.y / 2;
    maxPos.z += size.z / 2;

    if (Collision3D::isSphereInBox(player.size.y / 2, player.pos, minPos,
        maxPos)) {
        /* Sphere collided into platform and is not on top. We must determine
        from where we hit the platform. */
        glm::vec3 point{
            Collision3D::getBoxPointClosestToSphere(minPos, maxPos, player.pos)
        };

        /* Everything will be from camera point of view. For this game we will
        always see the platform coming to us. Hence we will use the following
        notation:
            - back side of the box is the side we see coming to us
            - front side of the box is the opposite
            - left side is left to us
            - right side is right to us
        */

        // Back side of the box.
        glm::vec3 backSideMinPos    { maxPos };
        glm::vec3 backSideMaxPos    { maxPos };
        // Setting back side of the box.
        backSideMinPos.x -= size.x;
        backSideMinPos.y -= size.y;

        // Front side of the box.
        glm::vec3 frontSideMinPos   { minPos };
        glm::vec3 frontSideMaxPos   { minPos };
        // Setting front side of the box.
        frontSideMaxPos.x += size.x;
        frontSideMaxPos.y += size.y;

        // Left side of the box.
        glm::vec3 leftSideMinPos    { minPos };
        glm::vec3 leftSideMaxPos    { minPos };
        // Setting left side of the box.
        leftSideMaxPos.y += size.y;
        leftSideMaxPos.z += size.z;
        
        // Right side of the box.
        glm::vec3 rightSideMinPos   { maxPos };
        glm::vec3 rightSideMaxPos   { maxPos };
        // Setting right side of the box.
        rightSideMinPos.y -= size.y;
        rightSideMinPos.z -= size.z;

        // Down side of the box.
        glm::vec3 downSideMinPos    { minPos };
        glm::vec3 downSideMaxPos    { minPos };
        // Setting down side of the box.
        downSideMaxPos.x += size.x;
        downSideMaxPos.z += size.z;

        // Up side of the box.
        glm::vec3 upSideMinPos    { maxPos };
        glm::vec3 upSideMaxPos    { maxPos };
        // Setting up side of the box.
        upSideMinPos.x -= size.x;
        upSideMinPos.z -= size.z;

        // Abilities
        if (!player.dead) {
            if (color == Constants::redPlatformColor) {
                if (player.orbits.amount > 0) {
                    --player.orbits.amount;
                } else {
                    player.dead = true;
                    player.respawn(Constants::playerInitialPos);
                    printf("\nMenu:\n");
                    printf("Press w,a,s,d to start playing.\n");
                    printf("Press ESC to quit.\n");
                }
            }
            if (color == Constants::yellowPlatformColor) {
                player.fuelTank.fuel = glm::max(0.0f,
                    player.fuelTank.fuel - Constants::yellowFuelPenalty);
                if (player.fuelTank.fuel == 0 && player.orbits.amount > 0) {
                    player.fuelTank.fuel = player.fuelTank.capacity;
                    --player.orbits.amount;
                }
            }
            if (color == Constants::orangePlatformColor) {
                if (player.mustResetSpeed) {
                    player.lockedSpeed = true;
                    player.mustResetSpeed = false;
                }
                if (player.lockedSpeed) {
                    // Just reset timer.
                    player.speedTimer = 0.0f;
                } else {
                    player.mustDeform = true;
                    player.lockedSpeed = true;
                }
            }
            if (color == Constants::greenPlatformColor) {
                player.fuelTank.fuel = glm::min(player.fuelTank.capacity,
                    player.fuelTank.fuel + Constants::greenBonusFuel);
                player.fuelTank.empty = false;
            }
        }


        // Testing point inside up side of the box.
        if (Collision3D::isPointInsideBox(point, upSideMinPos, upSideMaxPos)) {
            if (player.speed.y >= 0) {
                player.onPlatform = true;
            }
        }
        // Testing point inside back side of the box.
        if (Collision3D::isPointInsideBox(point, backSideMinPos, backSideMaxPos)) {
            player.collidedFront = true;
        }
        // Testing point inside front side of the box.
        if (Collision3D::isPointInsideBox(point, frontSideMinPos, frontSideMaxPos)) {
            player.collidedBack = true;
        }
        // Testing point inside left side of the box.
        if (Collision3D::isPointInsideBox(point, leftSideMinPos, leftSideMaxPos)) {
            player.collidedRight = true;
        }
        // Testing point inside right side of the box.
        if (Collision3D::isPointInsideBox(point, rightSideMinPos, rightSideMaxPos)) {
            player.collidedLeft = true;
        }
        // Testing point inside down side of the box.
        if (Collision3D::isPointInsideBox(point, downSideMinPos, downSideMaxPos)) {
            if (player.speed.y <= 0) {
                player.collidedUp = true;
            }
        }
        return true;
    }



    return false;
}

bool Platform3D::hitObject(Platform3D& platform) {
    return false;
}

bool Platform3D::hitByObject(Object3D& object) {
    return object.hitObject(*this);
}

void Platform3D::update(float deltaTimeSeconds) {
    // Updates visible boolean.
    checkVisible();

    // Updates expired boolean.
    checkExpired();

    if (!expired) {
        // Update position.
        pos.z += speed.z * deltaTimeSeconds;
    }
}