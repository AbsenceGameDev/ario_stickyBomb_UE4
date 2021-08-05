// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Delegates/Delegate.h>
#include <GameFramework/GameModeBase.h>

#include "StickyGameMode.generated.h"

/**
 * @brief Declare a new dynamic multicast delegate with three parameters
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * @brief Declare a new dynamic multicast delegate with four parameters
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTargetHitEvent, FVector, HitLocation, FVector, ShotDirection, float, HitValue, AActor*, HitOwner);

/**
 * @author Ario Amin  @file /StickyGameMode.h
 * @class AStickyGameMode
 * @brief Simple GameMode which has beend designed to work with ABaseShooter derived classes.
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new AStickyGameMode object
	 *
	 */
	AStickyGameMode();

	public:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief
	 *
	 */
	virtual void StartPlay() override;

	/**
	 * @brief
	 *
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief
	 *
	 * @param DeltaSeconds
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * @brief
	 *
	 * @param NewPlayer
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * @brief
	 *
	 * @param ExitingPlayer
	 */
	virtual void Logout(AController* ExitingPlayer) override;

	/* ================================ */
	/* Public Methods: Client Interface */

	/**
	 * @brief
	 *
	 * @param LocalPlayerId
	 * @return ABaseShooter*
	 */
	ABaseShooter* FindPlayer(int32 LocalPlayerId);

	/* =============================== */
	/* Public Fields: Events/Delegates */
	/**
	 * @brief OnActorKilled delegate, multicasts when a player has been killed
	 *
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnActorKilled OnActorKilled;

	/**
	 * @brief OnHitEvent Delegate, multicasts when a hit has occured
	 *
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnTargetHitEvent OnHitEvent;

	protected:
	/* ======================================== */
	/* Protected Methods: Server/Client Actions */

	/**
	 * @brief
	 *
	 * @param NewPlayer
	 */
	void RegisterNewPlayer(ABaseShooter* NewPlayer);

	/**
	 * @brief
	 *
	 * @param ExitingPlayer
	 */
	void DeregisterExitingPlayer(ABaseShooter* ExitingPlayer);

	/**
	 * @brief
	 *
	 */
	void CheckAnyPlayerAlive();

	/**
	 * @brief
	 *
	 * @return true
	 * @return false
	 */
	bool HasPlayerWon() const;

	/**
	 * @brief
	 *
	 */
	void RestartDeadPlayers();

	/**
	 * @brief
	 *
	 * @param DamageCauser
	 * @param DamagedActor
	 * @param InstigatorController
	 */
	UFUNCTION()
	void PlayerKilled(AActor* DamageCauser, AActor* DamagedActor, AController* InstigatorController);

	/**
	 * @brief
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
