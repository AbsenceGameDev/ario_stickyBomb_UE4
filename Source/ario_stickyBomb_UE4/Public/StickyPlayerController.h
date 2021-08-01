// Fill out your copyright notice in the Description page of Project Settings.

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
