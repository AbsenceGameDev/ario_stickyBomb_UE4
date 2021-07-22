// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "StickyGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
  FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyGameMode : public AGameModeBase
{
  GENERATED_BODY()

  public:
  AStickyGameMode();

  protected:
  void CheckAnyPlayerAlive();

  void GameOver();
  void RestartDeadPlayers();

  public:
  virtual void StartPlay() override;

  virtual void Tick(float DeltaSeconds) override;

  UPROPERTY(BlueprintAssignable, Category = "GameMode")
  FOnActorKilled OnActorKilled;
};
