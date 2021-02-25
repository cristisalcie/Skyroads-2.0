#pragma once

#include <skyroads/common/Collision3D.h>


glm::vec3 Collision3D::getBoxPointClosestToSphere(  glm::vec3 boxMinPos,
                                                    glm::vec3 boxMaxPos,
                                                    glm::vec3 spherePos) {
    glm::vec3 point{  };
    point.x = glm::max(boxMinPos.x, glm::min(spherePos.x, boxMaxPos.x));
    point.y = glm::max(boxMinPos.y, glm::min(spherePos.y, boxMaxPos.y));
    point.z = glm::max(boxMinPos.z, glm::min(spherePos.z, boxMaxPos.z));
    return point;    
}

bool Collision3D::isPointInsideBox( glm::vec3 point,
                                    glm::vec3 boxMinPos,
                                    glm::vec3 boxMaxPos) {
    return (point.x >= boxMinPos.x && point.x <= boxMaxPos.x)
        && (point.y >= boxMinPos.y && point.y <= boxMaxPos.y)
        && (point.z >= boxMinPos.z && point.z <= boxMaxPos.z);
}

bool Collision3D::isPointInsideSphere(  glm::vec3 point,
                                        float sphereRadius,
                                        glm::vec3 spherePos) {
    float distance;
    distance = glm::sqrt( (point.x - spherePos.x) * (point.x - spherePos.x)
                        + (point.y - spherePos.y) * (point.y - spherePos.y)
                        + (point.z - spherePos.z) * (point.z - spherePos.z));
    return distance < sphereRadius;
}

bool Collision3D::isSphereInBox(float sphereRadius,
                                glm::vec3 spherePos,
                                glm::vec3 boxMinPos,
                                glm::vec3 boxMaxPos) {
    glm::vec3 point {
        getBoxPointClosestToSphere(boxMinPos, boxMaxPos, spherePos)
    };
    return isPointInsideSphere(point, sphereRadius, spherePos);
}
