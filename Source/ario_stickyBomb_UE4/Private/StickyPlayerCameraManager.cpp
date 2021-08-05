/**
 * @author  Ario Amin
 * @file    /StickyPlayerCameraManager.cpp
 * @class   AStickyPlayerCameraManager
 * @brief   A simple shell of a class implementation to be expanded upon.
 * @details Only used to force 'bShouldSendClientSideCameraUpdate' to be true, as the default camera in the FPS example
 *          is not controlled by a Camera Manager, and thus doesn't have builtin support for replicating camera pitch.
 **/
#include "StickyPlayerCameraManager.h"

AStickyPlayerCameraManager::AStickyPlayerCameraManager() { bShouldSendClientSideCameraUpdate = true; }
