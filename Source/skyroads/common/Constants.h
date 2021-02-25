#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Constants {
    const glm::vec3 playerColor{ glm::vec3(1.0f, 0.65f, 0.0f) };

    const glm::vec3 playerInitialSize{
        glm::vec3(0.5f, 0.5f, 0.5f) };

    const glm::vec3 playerInitialPos{
        glm::vec3(0, 0, 0) };

    const float thirdPersonDistanceToTarget{ 3 };
    const float firstPersonDistanceToTarget{ -playerInitialSize.z / 2 };

    const glm::vec3 cameraThirdPersonOffset{
        glm::vec3(0.0f, 1.0f, thirdPersonDistanceToTarget) };

    const glm::vec3 cameraThirdPersonPos{
        cameraThirdPersonOffset + playerInitialPos };

    const glm::vec3 cameraFirstPersonOffset{
        glm::vec3(0.0f, 0.0f, firstPersonDistanceToTarget) };

    const glm::vec3 cameraFirstPersonPos{
        cameraFirstPersonOffset + playerInitialPos };


    const float gravity{ 10 }; // The smaller the value, the bigger the gravity
    const float playerJumpDistance{ 10 };
    const float playerHitCornerJumpDistance{ 0.025f };

    const float playerMaxSpeed{ 4.0f };

    // If the same as playerMaxSpeed, then we reach maxSpeed in 1 second.
    const float playerReachMaxSpeedTime{ 4.0f }; // Seconds.


    const float initialFieldOfView{ 60.0f };


    const float renderDistanceZ{ 160.0f };

    const float startingPlatformSideX{ 6.0f };
    const float startingPlatformSideZ{ 40.0f };
    const float startingPlatformHeight{ 0.5f };

    const float platformInitialSpeed{ 8.0f };


    const float platformMinSideX{ 0.5f };
    const float platformMaxSideX{ 2.0f };

    const float platformMinSideZ{ 20.0f };
    const float platformMaxSideZ{ 60.0f };

    const float platformMinHeightY{ startingPlatformHeight };
    const float platformMaxHeightY{ 3.5f };

    // Gap on Ox starting from platformMaxSideX so they won't overlay
    const float platformsGapX{ platformMaxSideX + 2.0f };

    const float platformsGapZ{ 6.0f };

    // Modifying this requires a new algorithm in Level.cpp at generation.
    const unsigned int platformMaxColumns{ 3 };

    const float platformMinSpawnX{
        playerInitialPos.x - platformsGapX * (platformMaxColumns / 2) };
    const float platformMaxSpawnX{
        platformMinSpawnX + platformsGapX * (platformMaxColumns - 1) };


    const glm::vec3 simplePlatformColor{ glm::vec3(0.0f, 0.0f, 1.0f) };
    const glm::vec3 hitPlatformColor{ glm::vec3(0.4f, 0.0f, 1.0f) };


    
    const glm::vec3 redPlatformColor{ glm::vec3(1.0f, 0.0f, 0.0f) };


    const glm::vec3 yellowPlatformColor{ glm::vec3(1.0f, 1.0f, 0.0f) };
    const float yellowFuelPenalty{ 10.0f };


    const glm::vec3 orangePlatformColor{ glm::vec3(1.0f, 0.65f, 0.0f) };
    const float orangePlatformSpeedTime{ 1.2f }; // Seconds.
    const float orangePlatformSpeed{ 20.0f }; // Speed.


    const glm::vec3 greenPlatformColor{ glm::vec3(0.0f, 1.0f, 0.0f) };
    const float greenBonusFuel{ 20.0f };



    const float playerDyingBelowY{ -12 };
    const float playerDeathBelowY{ -60 };





    /* It gets consumed as seconds when at lowest speed posible from player
    point of view. Otherwise it will be consumed faster depending on how much
    faster you are moving. */
    const float fuelTankCapacity{ 60 };




    const float asteroidsInitialSpeed{ 7.0f };

    const glm::vec3 asteroidsGap{ glm::vec3(5.5f, 9.0f, 10.0f) };

    const unsigned int asteroidMaxLines{ 5 };

    // Must be even. 7 columns to the left and 7 to the right.
    const unsigned int asteroidMaxColumns{ 14 };

    const float asteroidMinSideZ{ 1.0f };
    const float asteroidMaxSideZ{ 4.0f };

    const float asteroidMinSpawnX{
        playerInitialPos.x - asteroidsGap.x * (asteroidMaxColumns / 2) - 12 };
}
