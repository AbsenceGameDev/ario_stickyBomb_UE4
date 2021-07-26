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
  void SetCurve(UCurveFloat* InCurve);

  UFUNCTION()
  float GetDamageRadius();

  UFUNCTION()
  void SetDamageRadius(float InRadius);

  UFUNCTION()
  float GetDamageAmount();

  UFUNCTION()
  void SetDamageAmount(float InDamage);

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

  protected:
  void PlayTimeline();

  UFUNCTION()
  void ModulateColor(const float InterpValue);

  UFUNCTION()
  void TriggerExplosion();

  /** called when projectile hits something */
  UFUNCTION()
  void OnHit(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

#pragma region Components
  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UCurveFloat* StickyTimelineCurve;

  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UStaticMeshComponent* MeshComponentPtr;

  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  USceneComponent* RootSceneComp;

  // Timeline Direction enum
  UPROPERTY()
  TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

  // Declare a Timeline ptr, pased into Projectile through SetTimeline
  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  UTimelineComponent* StickyTimelineComp;
#pragma endregion

  private:
  FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

  /** Sphere collision component */
  UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
  USphereComponent* CollisionComp;

  // Projectile material
  UPROPERTY(VisibleDefaultsOnly, Category = Material)
  UMaterialInstanceDynamic* MeshMaterialInstance;

  /** Projectile movement component */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
  UProjectileMovementComponent* ProjectileMovement;

  // Projectile damage
  UPROPERTY(VisibleDefaultsOnly, Category = Damage)
  float DamageValue;

  // Projectile damage-radius
  UPROPERTY(VisibleDefaultsOnly, Category = Damage)
  float DamageRadius;
};
