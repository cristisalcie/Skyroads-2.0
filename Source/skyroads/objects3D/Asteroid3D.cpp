#include <skyroads/objects3D/Asteroid3D.h>
#include <skyroads/objects3D/Player3D.h>

Asteroid3D::Asteroid3D( FILE* logFile,
                        glm::vec3 pos,
                        glm::vec3 size,
                        glm::vec3 speed,
                        glm::vec3 color) {
    this->logFile = logFile;
    fprintf(logFile, "Created asteroid\n");
    this->pos = pos;
    this->size = size;
    this->speed = speed;
    initialSpeed = speed;
    visible = false;
    expired = false;
    this->color = color;
    mustDeform = (rand() % 100 > 6);
    rotationSpeed = initialSpeed.z;
    angle = 0.0f;
}

Asteroid3D::Asteroid3D( FILE* logFile,
                        unsigned int line,
                        unsigned int column,
                        glm::vec3 size,
                        glm::vec3 speed,
                        glm::vec3 color) {
    this->logFile = logFile;
    fprintf(logFile, "Created Asteroid\n");
    this->color = color;
    this->line = line;
    this->column = column;

    // Individual size for each asteroid
    int precision = 100;
    glm::vec3 newSize{
    (Constants::asteroidMinSideZ * precision
        + rand() % (int)
        ((Constants::asteroidMaxSideZ - Constants::asteroidMinSideZ)
            * precision))
        / precision
    };
    this->size = newSize;


    this->speed = speed;

    pos.x = Constants::asteroidMinSpawnX + (column - 1) * Constants::asteroidsGap.x;
    if (column > Constants::asteroidMaxColumns / 2) {
        pos.x += Constants::platformMaxColumns
            * (Constants::platformMaxSideX + Constants::platformsGapX);
        pos.x += 2 * Constants::asteroidsGap.x;  // A cheap simmetry.
    }
    pos.y = -Constants::asteroidMaxSideZ * 1.7f + (line - 1) * Constants::asteroidsGap.y;
    pos.z = -Constants::renderDistanceZ - Constants::asteroidsGap.z;

    initialSpeed = speed;
    visible = false;
    expired = false;

    mustDeform = (rand() % 100 > 6);
    rotationSpeed = initialSpeed.z;
    angle = 0.0f;
}

Asteroid3D::~Asteroid3D() {
    fprintf(logFile, "Deleted Asteroid\n");
}

void Asteroid3D::respawn(glm::vec3 pos) {
}

void Asteroid3D::respawn(unsigned int column, glm::vec3 size, glm::vec3 speed,
    glm::vec3 color) {
}

void Asteroid3D::respawn(unsigned int line, unsigned int column,
    glm::vec3 size, glm::vec3 speed, glm::vec3 color) {
    this->color = color;
    this->column = column;

    // Individual size for each asteroid
    int precision = 100;
    glm::vec3 newSize{
    (Constants::asteroidMinSideZ * precision
        + rand() % (int)
        ((Constants::asteroidMaxSideZ - Constants::asteroidMinSideZ)
            * precision))
        / precision
    };
    this->size = newSize;

    this->speed = speed;
    pos.x = Constants::asteroidMinSpawnX + (column - 1) * Constants::asteroidsGap.x;
    if (column > Constants::asteroidMaxColumns / 2) {
        pos.x += Constants::platformMaxColumns
            * (Constants::platformMaxSideX + Constants::platformsGapX);
        pos.x += 2 * Constants::asteroidsGap.x;  // A cheap simmetry.
    }
    pos.y = -Constants::asteroidMaxSideZ * 1.7f + (line - 1) * Constants::asteroidsGap.y;
    pos.z = -Constants::renderDistanceZ - Constants::asteroidsGap.z;


    visible = false;
    expired = false;
    mustDeform = (rand() % 100 > 6);
}

bool Asteroid3D::hitObject(Player3D& player) {
    return false; // No collisions.
}

bool Asteroid3D::hitObject(Platform3D& platform) {
    return false; // No collisions.
}

bool Asteroid3D::hitByObject(Object3D& object) {
    return false; // No collisions.
}

void Asteroid3D::update(float deltaTimeSeconds) {
    // Updates visible boolean.
    checkVisible();

    // Updates expired boolean.
    checkExpired();

    if (!expired) {
        // Update position.
        pos.z += speed.z * deltaTimeSeconds;
        angle += rotationSpeed * deltaTimeSeconds * 4;
        if (angle > 360) {
            angle = 0;
        }
    }
}

void Asteroid3D::checkVisible() {
    float frontPointZ = pos.z - size.z / 2;
    float backPointZ = pos.z + size.z / 2;

    // If platform passed player then it is not visible.
    if (frontPointZ > Constants::cameraThirdPersonPos.z + 48) {
        visible = false;
        return;
    }

    // If platform is out of render distance then it is not visible.
    if (backPointZ < -Constants::renderDistanceZ) {
        visible = false;
        return;
    }

    visible = true;
}

void Asteroid3D::checkExpired() {
    float frontPointZ = pos.z - size.z / 2;
    float backPointZ = pos.z + size.z / 2;

    // If platform passed player then it expired.
    if (frontPointZ > Constants::cameraThirdPersonPos.z + 48) {
        expired = true;
        return;
    }

    expired = false;
}
