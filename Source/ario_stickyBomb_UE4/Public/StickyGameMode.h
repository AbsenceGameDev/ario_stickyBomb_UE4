// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Delegates/Delegate.h>
#include <GameFramework/GameModeBase.h>

#include "StickyGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTargetHitEvent, FVector, HitLocation, FVector, ShotDirection, float, HitValue, AActor*, HitOwner);

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
	AStickyGameMode();

	public:
	/** ============================ **/
	/** Inherited Methods: Overrides **/
	virtual void StartPlay() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** =============================== **/
	/** Public Fields: Events/Delegates **/
	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnActorKilled OnActorKilled;

	UPROPERTY(BlueprintAssignable, Category = "Events: GameMode")
	FOnTargetHitEvent OnHitEvent;

	protected:
	/** ======================================== **/
	/** Protected Methods: Server/Client Actions **/
	void CheckAnyPlayerAlive();
	bool IsAnyPlayerAlive() const;
	void RestartDeadPlayers();
	void GameOver();

	/** ================================== **/
	/** Protected Fields: Basic properties **/
	bool bIsGameOver;
	bool bHasGameStarted;
};
