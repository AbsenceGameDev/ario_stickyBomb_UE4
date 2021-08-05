// Ario Amin - 2021/08

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Helpers/Macros.h"

#include <Engine/EngineTypes.h>

#include "AmmoComp.generated.h"

// OnAmmoChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnAmmoChangedSignature, UAmmoComp*, OwningAmmoComp, int, Ammo, int, AmmoDelta, AController*, InstigatedBy);

/**
 * @author  Ario Amin
 * @file    Components/AmmoComp.h
 * @class   UAmmoComp
 * @brief   Networked ammo component
 * @details Simple networked Ammo Component to be used for ABaseShooter derived actors
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UAmmoComp : public UActorComponent
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new UAmmoComp object
	 *
	 */
	UAmmoComp();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief   Component BeginPlay
	 * @details Runs when world at BeginPlay, or when constructed in an already running world.
	 *
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief   Unused, disabled in the constructor of the component
	 *
	 * @param   DeltaTime
	 * @param   TickType
	 * @param   ThisTickFunction
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	public:
	/* ================================ */
	/* Public Methods: Client interface */
	/**
	 * @brief   Try firing a round of ammo, calls server which makes sure it is allowed.
	 *
	 */
	UFUNCTION()
	void TryFire();

	/**
	 * @brief   Try picking up a round of ammo, calls server which makes sure it is allowed.
	 *
	 */
	UFUNCTION()
	void TryPickupRound();

	/* =============================== */
	/* Public Methods: Inlined Getters */

	/**
	 * @brief   Get the Ammo Count
	 *
	 * @return  float
	 */
	float GetAmmo() const { return AmmoCount; }

	/**
	 * @brief   Is the ammo component out of rounds?
	 *
	 * @return  true | false
	 */
	bool IsEmpty() const { return bIsEmpty; }

	/**
	 * @brief   Comparison of Ammo count,
	 * @details Returns result of comparison, true if AmmoCount is greater than or equal to MaxAmmo.
	 *
	 * @return  true | false
	 */
	bool IsFullClip() const { return AmmoCount >= MaxAmmo; }

	/* =============================== */
	/* Public Fields: Events/Delegates */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	protected:
	/* ======================================== */
	/* Protected Methods: Client/Server actions */

	/**
	 * @brief   OnFire, called by server
	 * @details Decrement the ammo count after request has been handles by server.
	 */
	UFUNCTION()
	void OnFire();

	/**
	 * @brief   OnPickupRound, called by server
	 * @details Increment the ammo count after request has been handles by server.
	 *
	 */
	UFUNCTION()
	void OnPickupRound();

	/**
	 * @brief   Server only, calls OnFire
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(); /** Send request to host server, withValidation */

	/**
	 * @brief   Server only, calls OnPickupRound
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnPickupRound(); /** Send request to host server, withValidation */

	/* ====================== */
	/* Protected Fields: Ammo */

	bool bIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoComp")
	int MaxAmmo = DEFAULT_STICKY_GUN_MAX;

	UPROPERTY(BlueprintReadOnly, Category = "AmmoComp")
	int AmmoCount = MaxAmmo;
};
