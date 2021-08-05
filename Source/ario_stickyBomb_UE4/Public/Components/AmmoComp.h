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
 * @author Ario Amin  @file Components/AmmoComp.h
 * @class UAmmoComp
 * @brief Networked ammo component
 * @details Simple networked Ammo Component to be used for ABaseShooter derived actors
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UAmmoComp : public UActorComponent
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new UAmmoComp object
	 *
	 */
	UAmmoComp();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief
	 *
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief
	 *
	 * @param DeltaTime
	 * @param TickType
	 * @param ThisTickFunction
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	public:
	/* ================================ */
	/* Public Methods: Client interface */
	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void TryFire();

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void TryPickupRound();

	/* =============================== */
	/* Public Methods: Inlined Getters */

	/**
	 * @brief Get the Ammo object
	 *
	 * @return float
	 */
	float GetAmmo() const { return AmmoCount; }

	/**
	 * @brief
	 *
	 * @return true
	 * @return false
	 */
	bool IsEmpty() const { return bIsEmpty; }

	/**
	 * @brief
	 *
	 * @return true
	 * @return false
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
	 * @brief
	 *
	 * @param PrevAmmo
	 */
	UFUNCTION()
	void OnRep_Ammo(int PrevAmmo);

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void OnFire();

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void OnPickupRound();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(); /** Send request to host server, withValidation */

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnPickupRound(); /** Send request to host server, withValidation */

	/* ======================================== */
	/* Protected Fields: Ammo */
	bool bIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoComp")
	int MaxAmmo = DEFAULT_STICKY_GUN_MAX;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "AmmoComp")
	int AmmoCount = MaxAmmo;
};
