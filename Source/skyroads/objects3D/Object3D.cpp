#include <skyroads/objects3D/Object3D.h>

Object3D::Object3D() {
    logFile = nullptr;
    pos = glm::vec3(0);
    size = glm::vec3(1);
    color = glm::vec3(1);
    speed = glm::vec3(0);
    initialSpeed = glm::vec3(0);
}

Object3D::~Object3D() {
}