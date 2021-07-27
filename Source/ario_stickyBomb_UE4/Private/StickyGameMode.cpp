// Fill out your copyright notice in the Description page of Project Settings.

#include "StickyGameMode.h"

#include "Characters/Bomberman.h"
#include "Components/HealthComp.h"
#include "EngineUtils.h"
#include "StickyHUD.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"


AStickyGameMode::AStickyGameMode() : Super()
{
  // set default pawn class to our ABomberman
  DefaultPawnClass = ABomberman::StaticClass();

  // use our custom HUD class
  HUDClass = AStickyHUD::StaticClass();

  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;
}

void AStickyGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
	  APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
		  APawn* MyPawn = PC->GetPawn();
		  UHealthComp* HealthComp = Cast<UHealthComp>(MyPawn->GetComponentByClass(UHealthComp::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
			  // A player is still alive.
			  // UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"PLAYERS ALIVE!\""));
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
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
	  APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr) {
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
