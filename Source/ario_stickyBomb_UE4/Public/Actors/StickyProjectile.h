// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StickyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyProjectile : public AActor
{
  GENERATED_BODY()

  /** Sphere collision component */
  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  USphereComponent* CollisionComp;

  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UStaticMeshComponent* MeshComponentPtr;

  // Projectile material
  UPROPERTY(VisibleDefaultsOnly, Category = Movement)
  UMaterialInstanceDynamic* MeshMaterialInstance;

  /** Projectile movement component */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
  UProjectileMovementComponent* ProjectileMovement;

  public:
  AStickyProjectile();

  /** called when projectile hits something */
  UFUNCTION()
  void OnHit(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

  /** called when projectile gets picked up/interacted with */
  UFUNCTION()
  bool DidPickUp(AActor* OtherActor);

  /** Returns CollisionComp subobject **/
  USphereComponent* GetCollisionComp() const
  {
	return CollisionComp;
  }
  /** Returns ProjectileMovement subobject **/
  UProjectileMovementComponent* GetProjectileMovement() const
  {
	return ProjectileMovement;
  }
};
