// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseShooter.h"
#include "Components/AmmoComp.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoreMinimal.h"

#include "StickyGunSkeletalComp.generated.h"

/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API UStickyGunSkeletalComp : public USkeletalMeshComponent
{
  GENERATED_BODY()

  protected:
  UStickyGunSkeletalComp();

  public:
  /** Location on gun mesh where projectiles should spawn. */
  UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
  USceneComponent* PlacementComp;

  ABaseShooter* OwningCharacter;

  UAmmoComp* AmmoComp;

  bool bDisable = true;

  UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
  USkeletalMesh* MeshPtr;

  /** Gun muzzle's offset from the characters location */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
  FVector GunOffset;

  /** Projectile class to spawn */
  UPROPERTY(EditDefaultsOnly, Category = Projectile)
  TSubclassOf<class AStickyProjectile> ProjectileClass;

  /** Sound to play each time we fire */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
  USoundBase* FireSound;

  /** AnimMontage to play each time we fire */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
  UAnimMontage* FireAnimation;

  /** Fires a projectile. */
  void OnFire();

  /** Set-up sticky gun, should be right after called after constructing StickyGunSkeletalComp as a subobject */
  void InitStickyGun(ABaseShooter* Caller, FVector GunOffset, USceneComponent* MuzzlePlacementComp);

  UAmmoComp* GetAmmoCompPtr();
};
