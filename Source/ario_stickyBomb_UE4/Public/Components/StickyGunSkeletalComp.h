// Ario Amin - 2021/08

#pragma once

#include "Characters/BaseShooter.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/SkeletalMeshComponent.h>

#include "StickyGunSkeletalComp.generated.h"

/**
 * @author  Ario Amin
 * @file    Components/StickyGunSkeletalComp.h
 * @class   UStickyGunSkeletalComp
 * @brief   Networked weapon component
 * @details Derived from USkeletalMeshComponent,
 *          Designed to be used with ABaseShooter derived classes
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API UStickyGunSkeletalComp : public USkeletalMeshComponent
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new UStickyGunSkeletalComp object
	 *
	 */
	UStickyGunSkeletalComp();

	/* ================================ */
	/* Public Methods: Client interface */

	/**
	 * @brief Called after constructing StickyGunSkeletalComp as a subobject
	 *
	 * @param Caller
	 * @param GunOffset
	 * @param MuzzlePlacementComp
	 */
	void InitStickyGun(ABaseShooter* Caller, FVector GunOffset, USceneComponent* MuzzlePlacementComp);

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void TryStartFire();

	/* =============================== */
	/* Public Methods: Getters/Setters */

	/**
	 * @brief Get the Ammo Comp object
	 *
	 * @return UAmmoComp*
	 */
	UAmmoComp* GetAmmoComp();

	/**
	 * @brief Get the Owning Character object
	 *
	 * @return ABaseShooter*
	 */
	ABaseShooter* GetOwningCharacter();

	/**
	 * @brief Get the Fire Sound object
	 *
	 * @return USoundBase*
	 */
	USoundBase* GetFireSound();

	/**
	 * @brief Get the Fire Anim Montage object
	 *
	 * @return UAnimMontage*
	 */
	UAnimMontage* GetFireAnimMontage();

	/* ================================= */
	/* Public Methods: Networked VFX/SFX */

	/**
	 * @brief Multicasted VFX/SFX trigger
	 *
	 * @param LocalProjectileActorPtr
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireGunEffects(AStickyProjectile* LocalProjectileActorPtr);

	protected:
	/* ========================== */
	/* Protected Methods: VFX/SFX */

	/**
	 * @brief Succeeded with fire effcts
	 *
	 */
	void SuccessFireEffects();

	/**
	 * @brief Failed with fire effcts
	 *
	 */
	void FailFireEffects();

	/* ======================================== */
	/* Protected Methods: Client/Server actions */

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire();

	/**
	 * @brief
	 *
	 */
	void OnFire();

	/**
	 * @brief
	 *
	 * @param LocalProjectileActorPtr
	 */
	void PrepDeferredSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr);

	/**
	 * @brief
	 *
	 * @param LocalProjectileActorPtr
	 * @param SpawnTransform
	 * @return true
	 * @return false
	 */
	bool FinishSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr, FTransform const& SpawnTransform);

	/* ================================= */
	/* Protected Methods: Client actions */

	/**
	 * @brief
	 *
	 */
	void GenerateCurve();

	/* =================================== */
	/* Protected Methods: Basic properties */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMesh* MeshPtr = nullptr; /** Actual Skeletal Mesh asset */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset; /** Gun muzzle's offset from the characters location */

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<AStickyProjectile> ProjectileClass; /** Projectile class to spawn */

	/* ========================= */
	/* Protected Fields: VFX/SFX */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound; /** Sound to play each time we fire */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation; /** AnimMontage to play each time we fire */

	/* ============================ */
	/* Protected Fields: Components */
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	USceneComponent* PlacementComp; /** Spawn-point for projectiles */

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	UAmmoComp* AmmoComp;

	/* ================================== */
	/* Protected Fields: Basic Properties */
	ABaseShooter* OwningCharacter;

	// UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	// TSharedPtr<UCurveFloat> FloatCurve = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	UCurveFloat* FloatCurve = nullptr;

	// UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	FRichCurve* GeneratedRichCurve;
};
