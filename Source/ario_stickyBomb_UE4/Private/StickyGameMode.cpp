// Ario Amin - 2021/08

#include "StickyGameMode.h"

// Player
#include "Characters/Bomberman.h"
#include "StickyGameState.h"
#include "StickyPlayerController.h"
#include "StickyPlayerState.h"

// Components
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"

// UI
#include "UI/StickyHUD.h"

// Custom
#include "Effects/MaterialGenerator.h"

// Engine
#include <Engine/World.h>
#include <EngineUtils.h>
#include <GameFramework/Actor.h>
#include <TimerManager.h>

// Engine Helpers
#include <UObject/ConstructorHelpers.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

AStickyGameMode::AStickyGameMode() : Super()
{
	// set default pawn class to our ABomberman
	DefaultPawnClass			= ABomberman::StaticClass();
	PlayerStateClass			= AStickyPlayerState::StaticClass();
	GameStateClass				= AStickyGameState::StaticClass();
	PlayerControllerClass = AStickyPlayerController::StaticClass();

	OnActorKilled.AddDynamic(this, &AStickyGameMode::PlayerKilled);

	/* Generate Material during compilation, doing it in the gamemode was an arbitrary decision.
	 * Anywhere where it can compile into an object would have worked */
	MatGen = FMaterialGenerator::CreateObject();

	// use our custom HUD class
	HUDClass = AStickyHUD::StaticClass();

	bIsGameOver			= false;
	bHasGameStarted = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyGameMode::StartPlay()
{
	Super::StartPlay();

	if (!bTriggerOnce) {
		GEngine->Exec(nullptr, TEXT("log LogTimeline off"));
		MatGen->CreateCelShadedExplosionMat();
		bTriggerOnce = true;
	}
}
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
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Player Unique ID on Server: %i"), NewPlayer->GetUniqueID());
#endif		// STICKY_DEBUG
	int Steps = 0;
	for (auto& PlayerState : GetGameState<AStickyGameState>()->PlayerArray) {
#ifdef STICKY_DEBUG
		UE_LOG(LogTemp, Warning, TEXT("PlayerState#%i PlayerID: %i"), Steps, PlayerState->GetPlayerId());
#endif		// STICKY_DEBUG
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

void AStickyGameMode::PlayerKilled(AActor* DamagedActor, AActor* DamageCauser, AController* InstigatorController)
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"PLAYER KILLED!\""));
#endif		// STICKY_DEBUG
	auto CastBaseShooter = StaticCast<ABaseShooter*>(DamagedActor);
	if (IsValid(CastBaseShooter)) {
		CastBaseShooter->ServerTriggerRagdoll();
	}
}

void AStickyGameMode::GameOver()
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"GAME OVER!\""));
#endif		// STICKY_DEBUG
	bIsGameOver = true;
}
