// Copyright Epic Games, Inc. All Rights Reserved.

#include "ario_stickyBomb_UE4GameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "ario_stickyBomb_UE4Character.h"
#include "ario_stickyBomb_UE4HUD.h"

Aario_stickyBomb_UE4GameMode::Aario_stickyBomb_UE4GameMode() : Super()
{
  DefaultPawnClass = Aario_stickyBomb_UE4Character::StaticClass();

  // use our custom HUD class
  HUDClass = Aario_stickyBomb_UE4HUD::StaticClass();
}
