#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Collision3D {
    glm::vec3 getBoxPointClosestToSphere(   glm::vec3 boxMinPos,
                                            glm::vec3 boxMaxPos,
                                            glm::vec3 spherePos);

    bool isPointInsideBox(  glm::vec3 point,
                            glm::vec3 boxMinPos,
                            glm::vec3 boxMaxPos);

    bool isPointInsideSphere(   glm::vec3 point,
                                float sphereRadius,
                                glm::vec3 spherePos);

    bool isSphereInBox( float sphereRadius,
                        glm::vec3 spherePos,
                        glm::vec3 boxMinPos,
                        glm::vec3 boxMaxPos);
}
