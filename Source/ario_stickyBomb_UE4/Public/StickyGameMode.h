// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Delegates/Delegate.h>
#include <GameFramework/GameModeBase.h>

#include "StickyGameMode.generated.h"

/** @brief   Declare a new dynamic multicast delegate with three parameters */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/** @brief   Declare a new dynamic multicast delegate with four parameters */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTargetHitEvent, FVector, HitLocation, FVector, ShotDirection, float, HitValue, AActor*, HitOwner);

/**
 * @author  Ario Amin
 * @file    /StickyGameMode.h
 * @class   AStickyGameMode
 * @brief   Simple GameMode which has been designed to work with ABaseShooter derived classes.
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new AStickyGameMode object
	 *
	 */
	AStickyGameMode();

	public:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief   Plays when gamemode starts
	 * @details Generates a material using FMaterialGenerator and turns off timeline logs
	 *
	 */
	virtual void StartPlay() override;

	/**
	 * @brief   Unchanged, only calls Super::BeginPlay
	 *
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief   This gamemodes tick. uses it to keep track of the players and the game.
	 *
	 * @param   DeltaSeconds
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * @brief   Called when user logs-in
	 * @todo    Register user-chosen names?
	 *
	 * @param   NewPlayer
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * @brief   Called when user is exiting the game.
	 *
	 * @param   ExitingPlayer
	 */
	virtual void Logout(AController* ExitingPlayer) override;

	/* ================================ */
	/* Public Methods: Client Interface */

	/**
	 * @brief   Find player on server given player ID sent by client.
	 *
	 * @param   LocalPlayerId
	 * @return  ABaseShooter*
	 */
	ABaseShooter* FindPlayer(int32 LocalPlayerId);

	/* =============================== */
	/* Public Fields: Events/Delegates */
	/**
	 * @brief   OnActorKilled delegate, multicasts when a player has been killed
	 *
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnActorKilled OnActorKilled;

	/**
	 * @brief   OnHitEvent Delegate, multicasts when a hit has occured
	 *
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnTargetHitEvent OnHitEvent;

	protected:
	/* ======================================== */
	/* Protected Methods: Server/Client Actions */

	/**
	 * @brief   Register newly logged ing player
	 *
	 * @param   NewPlayer
	 */
	void RegisterNewPlayer(ABaseShooter* NewPlayer);

	/**
	 * @brief   Unregister player who is logging out
	 *
	 * @param   ExitingPlayer
	 */
	void DeregisterExitingPlayer(ABaseShooter* ExitingPlayer);

	/**
	 * @brief   Is any player alive?
	 * @todo    remove, superflous function
	 *
	 */
	void CheckAnyPlayerAlive();

	/**
	 * @brief   Checks whether match is still going of if has been won.
	 *
	 * @return  true | false
	 */
	bool HasPlayerWon() const;

	/**
	 * @brief   Restart any dead players who are still connected to the server.
	 * @todo    Reset player capsule orientation and reset stance?
	 *
	 */
	void RestartDeadPlayers();

	/**
	 * @brief   Registers a killed player
	 * @details Does not do much right now, but could be expanded upon to trigger OnKill audio cues or something similar
	 *
	 * @param   DamageCauser
	 * @param   DamagedActor
	 * @param   InstigatorController
	 */
	UFUNCTION()
	void PlayerKilled(AActor* DamageCauser, AActor* DamagedActor, AController* InstigatorController);

	/**
	 * @brief   Ends the current game.
	 *
	 */
	void GameOver();

	/* ================================== */
	/* Protected Fields: Basic properties */
	bool bIsGameOver;
	bool bHasGameStarted;
	bool bTriggerOnce = false;

	FMaterialGenerator* MatGen;
};
