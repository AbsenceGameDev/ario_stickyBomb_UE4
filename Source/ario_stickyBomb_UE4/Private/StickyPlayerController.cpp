/**
 * @author  Ario Amin
 * @file    /StickyPlayerController.cpp
 * @class   AStickyPlayerController
 * @brief   A simple shell of a class implementation to be expanded upon.
 * @todo    Move controller and input related code from the ABaseShooter class into here.
 */
#include "StickyPlayerController.h"

#include "StickyPlayerCameraManager.h"

AStickyPlayerController::AStickyPlayerController() { PlayerCameraManagerClass = AStickyPlayerCameraManager::StaticClass(); }