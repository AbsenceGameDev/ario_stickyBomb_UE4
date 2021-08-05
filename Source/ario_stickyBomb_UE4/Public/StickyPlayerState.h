
#pragma once

#include "CoreMinimal.h"
#include "StickyGameMode.h"

#include <GameFramework/PlayerState.h>

#include "StickyPlayerState.generated.h"

/**
 * @author  Ario Amin
 * @file    /StickyPlayerState.h
 * @class   AStickyPlayerState
 * @brief   Networked Player-state definition, derives from AStickyPlayerState
 * @details Keeps track of the state of the player, in regards to kills and ammo.
 *          Shares data to the server through the gamemode->gamestate
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyPlayerState : public APlayerState
{
	GENERATED_BODY()

	public:
	/* ================================ */
	/* Public Methods: Client interface */

	/**
	 * @brief
	 *
	 * @param DeltaScore
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddUserScore(float DeltaScore);

	/**
	 * @brief Get the User Score value
	 *
	 * @return float
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	float GetUserScore() const;

	/**
	 * @brief
	 *
	 * @param KillCount
	 */
	UFUNCTION(BlueprintCallable, Category = "Kills")
	void AddKill(int32 KillCount);

	/**
	 * @brief Get the Kills value
	 *
	 * @return int32
	 */
	UFUNCTION(BlueprintCallable, Category = "Kills")
	int32 GetKills() const;

	/**
	 * @brief Set the Ammo count
	 *
	 * @param AmmoCount
	 */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetAmmo(int32 AmmoCount);

	/**
	 * @brief Get the Ammo count
	 *
	 * @return int32
	 */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetAmmo() const;

	protected:
	/* ======================================== */
	/* Protected Methods: Server/Client Actions */
	/**
	 * @brief On replication, Kills
	 *
	 */
	UFUNCTION()
	void OnRep_Kills();

	/**
	 * @brief OnReplication, Ammo
	 *
	 */
	UFUNCTION()
	void OnRep_Ammo();

	/* ================================== */
	/* Protected Fields: Basic properties */
	UPROPERTY(ReplicatedUsing = OnRep_Kills, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Kills")
	int32 Kills;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Ammo")
	int32 Ammo;
};