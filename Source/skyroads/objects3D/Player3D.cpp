#include <skyroads/objects3D/Player3D.h>
#include <skyroads/objects3D/Platform3D.h>

Player3D::Player3D( FILE *logFile,
                    glm::vec3 pos,
                    glm::vec3 size,
                    glm::vec3 color) {
    this->logFile = logFile;

    mustDeform = false;

    rotationSpeed = 0;
    angle = 0;

    this->color = color;

    orbits.amount = 3;
    orbits.scale = glm::vec3(0.15f);
    orbits.radius = 0.4f;
    orbits.rotationSpeed = 1.6f;
    orbits.angle = 0.0f;

    fuelTank.capacity = Constants::fuelTankCapacity;
    fuelTank.fuel = fuelTank.capacity;
    fuelTank.empty = false;

    bounceBack = 0;

    lockedSpeed = false;
    speedTimer = -1.0f;
    lockSpeed = 0.0f;
    mustResetSpeed = false;

    dead = false;
    dying = false;

    this->pos   = pos;
    this->size  = size;
    speed = glm::vec3(0);
    initialSpeed = speed;
    onPlatform       = false;
    collidedFront    = false;
    collidedBack     = false;
    collidedLeft     = false;
    collidedRight    = false;
    collidedUp       = false;

    fprintf(logFile, "Created player\n");
}

Player3D::~Player3D() {
    fprintf(logFile, "Deleted player\n");
}


bool Player3D::hitObject(Player3D& player) {
    return false;
}

bool Player3D::hitObject(Platform3D& platform) {
    // Note!! Platform's hitObject will run before this.
    glm::vec3 minPos{ platform.pos };
    glm::vec3 maxPos{ platform.pos };

    float sectionPlatform = platform.size.y / 40;
    minPos.x -= platform.size.x / 2;
    minPos.y -= platform.size.y / 2;
    minPos.z -= platform.size.z / 2;

    maxPos.x += platform.size.x / 2;
    maxPos.y += platform.size.y / 2;
    maxPos.z += platform.size.z / 2;

    if (Collision3D::isSphereInBox(size.y / 2, pos, minPos,
        maxPos)) {
        // Affect platform by changing it's color.
        if (platform.color != Constants::hitPlatformColor) {
            platform.color = Constants::hitPlatformColor;
        }
        return true;
    }
    return false;
}

bool Player3D::hitByObject(Object3D& object) {
    return object.hitObject(*this);
}

void Player3D::respawn(glm::vec3 pos) {
    this->pos = pos;
    // Size stays the same.

    mustDeform = false;

    rotationSpeed = 0;
    angle = 0;

    fuelTank.fuel = fuelTank.capacity;
    fuelTank.empty = false;

    bounceBack = 0;

    speedTimer = -1.0f;
    lockSpeed = 0.0f;

    speed = glm::vec3(0);
    initialSpeed = speed;

    orbits.amount = 3;
    orbits.scale = glm::vec3(0.15f);
    orbits.radius = 0.4f;
    orbits.rotationSpeed = 1.6f;
    orbits.angle = 0.0f;

    lockedSpeed     = false;
    mustResetSpeed  = false;
    onPlatform      = false;
    collidedFront   = false;
    collidedBack    = false;
    collidedLeft    = false;
    collidedRight   = false;
    collidedUp      = false;
}

void Player3D::update(float deltaTimeSeconds) {
    if (dead) return;

    // Rotations logic
    {
        angle -= rotationSpeed * deltaTimeSeconds * 31;
        if (angle > 360) {
            angle = 0;
        }
        orbits.angle += deltaTimeSeconds * 100;
        if (orbits.angle > 360) {
            orbits.angle = 0;
        }  
    }
    // ------------------------------------------------------------------------


    // Fuel logic
    {
        if (!fuelTank.empty && !lockedSpeed && !dying && !mustResetSpeed) {
            float fuelConsumption = glm::max(1.0f, speed.z);
            float consumedFuel = deltaTimeSeconds * fuelConsumption / 1.5f;
            fuelTank.fuel = glm::max(0.0f, fuelTank.fuel - consumedFuel);
            if (fuelTank.fuel == 0 && orbits.amount > 0) {
                fuelTank.fuel = fuelTank.capacity;
                --orbits.amount;
            }
            if (fuelTank.fuel == 0.0f) {
                fuelTank.empty = true;
            }
        }
    }
    // ------------------------------------------------------------------------

    // Platform collisions.
    {
        if (collidedUp) {
            speed.y *= -1;
        }

        if (onPlatform) {
            if (collidedFront) {
                // Hit top corner of platform. Simulate jump.
                bounceBack = Constants::playerJumpDistance * deltaTimeSeconds;
                speed.y = -Constants::playerJumpDistance * deltaTimeSeconds;
                onPlatform = false;
            } else {
                bounceBack /= 3;
                if (bounceBack < 0.004) {
                    /* So it bounces less times (will be easier to play). It
                    also ensures it will not go negative.*/
                    bounceBack = 0;
                }
                speed.y = -bounceBack;
            }
        } else if (collidedUp && collidedFront) {
            // Hit corner of platform. Simulate jump.
            speed.y = Constants::playerJumpDistance * deltaTimeSeconds;
        } else {
            speed.y += deltaTimeSeconds / Constants::gravity;
            bounceBack = speed.y;
        }
    }
    // ------------------------------------------------------------------------


    // Orange platform hit case.
    {
        if (lockedSpeed) {
            if (speedTimer == -1) {
                // This is the time right before speed increase.
                lockSpeed = speed.z;

                // Initiate timer to start.
                speedTimer = 0.0f;
            }

            // Smooth transition.
            speed.z = glm::min(speed.z + deltaTimeSeconds * 20,
                Constants::orangePlatformSpeed);


            if (speed.z == Constants::orangePlatformSpeed) {
                speedTimer += deltaTimeSeconds; // Start timer.
            }

            if (speedTimer > Constants::orangePlatformSpeedTime) {
                // Check if reached end.
                speedTimer = -1.0f; // Timer ended. Back to unused state.
                lockedSpeed = false;
                mustResetSpeed = true;
            }
        } else if (mustResetSpeed) {

            speed.z = glm::max(speed.z - deltaTimeSeconds * 20,
                lockSpeed);

            if (speed.z == lockSpeed) {
                mustDeform = false;
                mustResetSpeed = false;
            }
        }
    }
    // ------------------------------------------------------------------------
    

    // Update position on Oy axis and handle death situation.
    {
        if (pos.y > Constants::playerDeathBelowY) {
            if (pos.y <= Constants::playerDyingBelowY) {
                if (!dying) {
                    dying = true;
                }
                pos.z -= speed.z * deltaTimeSeconds;
            }
            pos.y -= speed.y;
        } else {
            dying = false;
            dead  = true;
            respawn(Constants::playerInitialPos);
            printf("\nMenu:\n");
            printf("Press w,a,s,d to start playing.\n");
            printf("Press ESC to quit.\n");
            return;
        }
    }
    // ------------------------------------------------------------------------


    // Reset colliding booleans to be clean for next frame checks.
    {
        onPlatform       = false;
        collidedFront    = false;
        collidedBack     = false;
        collidedLeft     = false;
        collidedRight    = false;
        collidedUp       = false;
    }
    // ------------------------------------------------------------------------
}
