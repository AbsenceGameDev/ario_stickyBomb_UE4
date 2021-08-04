// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "StickyPlayerController.generated.h"

/**
 *
 */
UCLASS(config = game)
class ARIO_STICKYBOMB_UE4_API AStickyPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
	AStickyPlayerController();
};
