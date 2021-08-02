// Fill out your copyright notice in the Description page of Project Settings.

#include "StickyGameMode.h"

#include "Characters/Bomberman.h"
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "StickyGameState.h"
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
	GameStateClass				= AStickyGameState::StaticClass();
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

void AStickyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	auto NewBaseShooter = StaticCast<ABaseShooter*>(NewPlayer->GetPawn());
	if (NewBaseShooter) {
		if (GEngine) {
			// Register player
			RegisterNewPlayer(NewBaseShooter);
		}
	}
}

void AStickyGameMode::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);
	auto ExitingBaseShooter = StaticCast<ABaseShooter*>(ExitingPlayer->GetPawn());
	if (ExitingBaseShooter) {
		if (GEngine) {
			// De-register player
			DeregisterExitingPlayer(ExitingBaseShooter);
		}
	}
}

ABaseShooter* AStickyGameMode::FindPlayer(int32 LocalPlayerId)
{
	int Steps = 0;

	for (auto& PlayerState : GetGameState<AStickyGameState>()->PlayerArray) {
		int32 _LocalId = PlayerState->GetPlayerId();
		if (LocalPlayerId == _LocalId) {
			return PlayerState->GetPawn<ABaseShooter>();
		}

		Steps++;
	}
	return nullptr;
}
/** ======================================== **/
/** Protected Methods: Server/Client Actions **/
void AStickyGameMode::RegisterNewPlayer(ABaseShooter* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Unique ID on Server: %i"), NewPlayer->GetUniqueID());
	int Steps = 0;
	for (auto& PlayerState : GetGameState<AStickyGameState>()->PlayerArray) {
		UE_LOG(LogTemp, Warning, TEXT("PlayerState#%i PlayerID: %i"), Steps, PlayerState->GetPlayerId());
		Steps++;
	}
	// Access gamestate, do something with player-state array?
}
void AStickyGameMode::DeregisterExitingPlayer(ABaseShooter* ExitingPlayer)
{
	// Access gamestate, do something with player-state array?
}
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
