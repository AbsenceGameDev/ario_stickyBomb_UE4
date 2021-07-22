// Fill out your copyright notice in the Description page of Project Settings.

#include "StickyGameMode.h"

#include "../ario_stickyBomb_UE4Character.h"
#include "../ario_stickyBomb_UE4HUD.h"
#include "Components/HealthComp.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"


AStickyGameMode::AStickyGameMode() : Super()
{
  // set default pawn class to our Blueprinted character
  static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
	TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
  DefaultPawnClass = PlayerPawnClassFinder.Class;

  // use our custom HUD class
  HUDClass = Aario_stickyBomb_UE4HUD::StaticClass();

  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;
}

void AStickyGameMode::CheckAnyPlayerAlive()
{
  for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
  {
	APlayerController* PC = It->Get();
	if (PC && PC->GetPawn())
	{
	  APawn* MyPawn = PC->GetPawn();
	  UHealthComp* HealthComp = Cast<UHealthComp>(MyPawn->GetComponentByClass(UHealthComp::StaticClass()));
	  if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
	  {
		// A player is still alive.
		return;
	  }
	}
  }

  // No player alive
  GameOver();
}

void AStickyGameMode::GameOver()
{
  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"GAME OVER!\""));
}

void AStickyGameMode::RestartDeadPlayers()
{
  for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
  {
	APlayerController* PC = It->Get();
	if (PC && PC->GetPawn() == nullptr)
	{
	  RestartPlayer(PC);
	}
  }
}

void AStickyGameMode::StartPlay()
{
  Super::StartPlay();
}

void AStickyGameMode::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);
  CheckAnyPlayerAlive();
}
