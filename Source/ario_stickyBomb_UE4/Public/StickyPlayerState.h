
#pragma once

#include "CoreMinimal.h"
#include "StickyGameMode.h"

#include <GameFramework/PlayerState.h>

#include "StickyPlayerState.generated.h"

/*
	Replicated Class Data that holds Kills, Score, Player name
*/

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyPlayerState : public APlayerState
{
  GENERATED_BODY()

  UFUNCTION()
  void OnRep_Kills();

  UPROPERTY(ReplicatedUsing = OnRep_Kills, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Kills")
  int32 Kills;

  public:
  UFUNCTION(BlueprintCallable, Category = "Score")
  void AddUserScore(float DeltaScore);

  float GetUserScore() const;

  UFUNCTION(BlueprintCallable, Category = "Kills")
  void AddKill(int32 KillCount);

  int32 GetKills() const;
};