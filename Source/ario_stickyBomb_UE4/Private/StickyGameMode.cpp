// Fill out your copyright notice in the Description page of Project Settings.

#include "StickyGameMode.h"

#include "Characters/Bomberman.h"
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "StickyPlayerController.h"
#include "StickyPlayerState.h"
#include "UI/StickyHUD.h"

#include <Engine/World.h>
#include <EngineUtils.h>
#include <GameFramework/Actor.h>
#include <TimerManager.h>
#include <UObject/ConstructorHelpers.h>
#include <Widgets/DeclarativeSyntaxSupport.h>


AStickyGameMode::AStickyGameMode() : Super()
{
	// set default pawn class to our ABomberman
	DefaultPawnClass			= ABomberman::StaticClass();
	PlayerStateClass			= AStickyPlayerState::StaticClass();
	PlayerControllerClass = AStickyPlayerController::StaticClass();

	// use our custom HUD class
	HUDClass = AStickyHUD::StaticClass();

	bIsGameOver			= false;
	bHasGameStarted = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyGameMode::StartPlay() { Super::StartPlay(); }
void AStickyGameMode::BeginPlay() { Super::BeginPlay(); }

// Tick Runs 60 times per sec / 1 for each frame
void AStickyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsGameOver) {
		return;
	}
	if (bHasGameStarted && !IsAnyPlayerAlive()) {
		GameOver();
	}
}

/** ======================================== **/
/** Protected Methods: Server/Client Actions **/
void AStickyGameMode::CheckAnyPlayerAlive()
{
	// Switch to timer based perhaps, so players can respawn
	if (IsAnyPlayerAlive()) {
		return;
	}

	// No player alive
	GameOver();
}

bool AStickyGameMode::IsAnyPlayerAlive() const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		APlayerController* CurrentPlayerCtl = It->Get();
		if (CurrentPlayerCtl && CurrentPlayerCtl->GetPawn()) {
			APawn*			 Pawn				= CurrentPlayerCtl->GetPawn();
			UHealthComp* HealthComp = Cast<UHealthComp>(Pawn->GetComponentByClass(UHealthComp::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
				return true;
			}
		}
	}
	return false;
}

// only runs on the server, grabs all playerControllers
void AStickyGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		APlayerController* CurrentPlayerCtl = It->Get();
		if (CurrentPlayerCtl && CurrentPlayerCtl->GetPawn() == nullptr) {
			RestartPlayer(CurrentPlayerCtl);
		}
	}
}

void AStickyGameMode::GameOver()
{
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"GAME OVER!\""));
	bIsGameOver = true;
}
