// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseShooter.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/SkeletalMeshComponent.h>

#include "StickyGunSkeletalComp.generated.h"

/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API UStickyGunSkeletalComp : public USkeletalMeshComponent
{
	GENERATED_BODY()

	public:
	UStickyGunSkeletalComp();

	/** ================================ **/
	/** Public Methods: Client interface **/

	void InitStickyGun(ABaseShooter* Caller, FVector GunOffset, USceneComponent* MuzzlePlacementComp);
	/** Called after constructing StickyGunSkeletalComp as a subobject */

	UFUNCTION()
	void TryStartFire();

	/** =============================== **/
	/** Public Methods: Getters/Setters **/
	UAmmoComp*		GetAmmoComp();
	ABaseShooter* GetOwningCharacter();
	USoundBase*		GetFireSound();
	UAnimMontage* GetFireAnimMontage();

	/** ================================= **/
	/** Public Methods: Networked VFX/SFX **/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireGunEffects(AStickyProjectile* LocalProjectileActorPtr);

	protected:
	/** ========================== **/
	/** Protected Methods: VFX/SFX **/
	void SuccessFireEffects();
	void FailFireEffects();

	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire();

	void OnFire();
	void PrepDeferredSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr);
	bool FinishSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr, FTransform const& SpawnTransform);

	/** ================================= **/
	/** Protected Methods: Client actions **/
	void GenerateCurve();

	/** =================================== **/
	/** Protected Methods: Basic properties **/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMesh* MeshPtr = nullptr; /** Actual Skeletal Mesh asset */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset; /** Gun muzzle's offset from the characters location */

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<AStickyProjectile> ProjectileClass; /** Projectile class to spawn */

	/** ========================= **/
	/** Protected Fields: VFX/SFX **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound; /** Sound to play each time we fire */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation; /** AnimMontage to play each time we fire */

	/** ============================ **/
	/** Protected Fields: Components **/
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	USceneComponent* PlacementComp; /** Spawn-point for projectiles */

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	UAmmoComp* AmmoComp;

	/** ================================== **/
	/** Protected Fields: Basic Properties **/
	ABaseShooter* OwningCharacter;

	// UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	// TSharedPtr<UCurveFloat> FloatCurve = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	UCurveFloat* FloatCurve = nullptr;

	// UPROPERTY(VisibleDefaultsOnly, Category = Curves)
	FRichCurve* GeneratedRichCurve;

	bool bDisable = true;
};
