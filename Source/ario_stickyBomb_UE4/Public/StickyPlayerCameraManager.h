// Ario Amin - 2021/08

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "CoreMinimal.h"

#include "StickyPlayerCameraManager.generated.h"

/**
 * @author Ario Amin  @file /StickyPlayerCameraManager.h
 * @class AStickyPlayerCameraManager
 * @brief Dummy Camera Manager, derives from APlayerCameraManager
 * @details Only used to set pitch to replicate thought the camera manager,
 * instead of forcing replication of pitch on the tick of the player
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new AStickyPlayerCameraManager object
	 *
	 */
	AStickyPlayerCameraManager();
};
