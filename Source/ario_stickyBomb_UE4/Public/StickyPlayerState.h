
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

	public:
	/** ================================ **/
	/** Public Methods: Client interface **/
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddUserScore(float DeltaScore);

	UFUNCTION(BlueprintCallable, Category = "Score")
	float GetUserScore() const;

	UFUNCTION(BlueprintCallable, Category = "Kills")
	void AddKill(int32 KillCount);

	UFUNCTION(BlueprintCallable, Category = "Kills")
	int32 GetKills() const;

	protected:
	/** ======================================== **/
	/** Protected Methods: Server/Client Actions **/
	UFUNCTION()
	void OnRep_Kills();

	/** ================================== **/
	/** Protected Fields: Basic properties **/
	UPROPERTY(ReplicatedUsing = OnRep_Kills, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Kills")
	int32 Kills;
};