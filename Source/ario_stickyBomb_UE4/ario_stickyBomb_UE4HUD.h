// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ario_stickyBomb_UE4HUD.generated.h"

UCLASS()
class Aario_stickyBomb_UE4HUD : public AHUD
{
	GENERATED_BODY()

public:
	Aario_stickyBomb_UE4HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

