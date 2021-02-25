#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <skyroads/objects3D/Object3D.h>

class Player3D : public Object3D {
private:
    typedef struct {
        float capacity;
        float fuel;
        bool empty;
    } FuelTank;

    typedef struct {
        /* This is player lifes. However falling from platforms will still be
        instant death. */
        unsigned int amount;

        glm::vec3 scale;
        float rotationSpeed;
        float radius;
        float angle;
    } Orbits;

public:
    Player3D(   FILE* logFile,
                glm::vec3 pos = glm::vec3(0),
                glm::vec3 size = glm::vec3(1),
                glm::vec3 color = glm::vec3(Constants::playerColor));
    virtual ~Player3D();


    /* Deform this object when player is affected by an ability. Currently only
    on orange platform because it's the only one that has a duration. For that
    case, mustDeform gets set the exact same way as lockedSpeed and reset the
    same way as mustResetSpeed. */
    bool mustDeform;

    // Used to have a varying rotation on OY axis only. Gets updated in Level.cpp
    float rotationSpeed;
    float angle; // To rotate with rotationSpeed.

    Orbits orbits;

    /* Player won't be able to jump if fuel is empty, possibly causing him
    death unless he somehow lands on a green platform. */
    FuelTank fuelTank;

    float bounceBack;

    /* lockedSpeed is used to be set true when we hit orange platform.
    will be set to false after timer ends. */
    bool lockedSpeed;

    /* speedTimer will have value -1.0f when it is not used so we know to store
    in lockSpeed the speed.z before we set speed.z to the high speed. */
    float speedTimer;   // Used to time lockedSpeed. 
    float lockSpeed;    // Speed storage for the speed before lockedSpeed was true. 
    /* mustResetSpeed is to know whether we must set speed back to normal or not.
    It will get set to true when timer ends, and to false after we reset speed. */
    bool mustResetSpeed;


    /*Used to animate the dying situation. Will be initially false, and then
    will be set to true after a certain Z was reached. It will reset to false
    when dead becomes true. */
    bool dying;

    /* dead is used to determine whether player died on not. Initially set to
    false. It will be set to true when we go below a certain Z value, and it
    will help the game make a decision based on this boolean.
    (end game, or try again)
    Every class will look at this dead boolean and react accordingly.
    (camera, player, level, etc)
    It will be set to false when we press w,a,s,d. */
    bool dead;


    /* onPlatform means the player is on top of it so it hasn't collided with
    the platform. */
    bool onPlatform;

    /* collided means it hit an object (platform). If object is on
    platform, collided is false. */
    bool collidedFront; // Hit object to the front.
    bool collidedBack;  // Hit object to the back
    bool collidedLeft;  // Hit object to the left.
    bool collidedRight; // Hit object to the right.
    bool collidedUp;    // Hit object from downside upwards.

    void update(float deltaTimeSeconds) override;
    bool hitObject(Player3D& player) override;
    bool hitObject(Platform3D& platform) override;
    bool hitByObject(Object3D& object) override;
    void respawn(glm::vec3 pos) override;
};
