// Copyright Epic Games, Inc. All Rights Reserved.

#include "ario_stickyBomb_UE4GameMode.h"
#include "ario_stickyBomb_UE4HUD.h"
#include "ario_stickyBomb_UE4Character.h"
#include "UObject/ConstructorHelpers.h"

Aario_stickyBomb_UE4GameMode::Aario_stickyBomb_UE4GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = Aario_stickyBomb_UE4HUD::StaticClass();
}
