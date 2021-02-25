#include <skyroads/objects3D/LevelObject3D.h>

LevelObject3D::LevelObject3D() {
    visible = false;
    expired = false;
    column  = 1; 
    line    = 1;
}

LevelObject3D::~LevelObject3D() {
}

void LevelObject3D::checkVisible() {
    float frontPointZ = pos.z - size.z / 2;
    float backPointZ  = pos.z + size.z / 2;

    // If platform passed player then it is not visible.
    if (frontPointZ > Constants::cameraThirdPersonPos.z + 3) {
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

void LevelObject3D::checkExpired() {
    float frontPointZ = pos.z - size.z / 2;
    float backPointZ  = pos.z + size.z / 2;

    // If platform passed player then it expired.
    if (frontPointZ > Constants::cameraThirdPersonPos.z + 3) {
        expired = true;
        return;
    }

    expired = false;
}
