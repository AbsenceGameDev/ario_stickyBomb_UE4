// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "StickyPlayerController.generated.h"

/**
 * @author  Ario Amin
 * @file    /StickyPlayerController.h
 * @class   AStickyPlayerController
 * @brief   Dummy Player Controller, derives from APlayerController
 * @details This class is currently only used to explicitly set some default values
 * @todo    Move controller and input related code from ABaseShooter into the player Controller
 */
UCLASS(config = game)
class ARIO_STICKYBOMB_UE4_API AStickyPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new AStickyPlayerController object
	 *
	 */
	AStickyPlayerController();
};
