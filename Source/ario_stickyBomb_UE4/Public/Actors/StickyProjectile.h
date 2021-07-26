// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TimelineComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StickyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyProjectile : public AActor
{
  GENERATED_BODY()

  public:
  AStickyProjectile();

  virtual void BeginPlay() final;
  virtual void Tick(float DeltaTime) final;

  UFUNCTION()
  void ModulateColor(float InterpValue);

  UFUNCTION()
  void TriggerExplosion();

  UFUNCTION()
  void SetCurve(UCurveFloat* InCurve);

  void PlayTimeline();

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

  private:
  FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

  // Timeline Direction enum
  UPROPERTY()
  TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

  // Declare a Timeline ptr, pased into Projectile through SetTimeline
  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UTimelineComponent* StickyTimelineComp;

  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UCurveFloat* StickyTimelineCurve;

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
};
