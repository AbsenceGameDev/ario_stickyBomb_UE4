// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/TimelineComponent.h>
#include <GameFramework/Actor.h>

#include "StickyProjectile.generated.h"

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyProjectile : public AActor
{
	GENERATED_BODY()

	public:
	AStickyProjectile();

	protected:
	/** ============================ **/
	/** Inherited Methods: Overrides **/
	virtual void Tick(float DeltaTime) final;
	virtual void BeginPlay() final;
	virtual void LifeSpanExpired() final;

	public:
	/** =============================== **/
	/** Public Methods: Getters/Setters **/

	USphereComponent*							GetCollisionComp() const;
	UProjectileMovementComponent* GetProjectileMovement() const;

	UFUNCTION()
	float GetDamageRadius() const;

	UFUNCTION()
	float GetDamageAmount() const;

	UFUNCTION()
	void SetDamageRadius(float InRadius);

	UFUNCTION()
	void SetDamageAmount(float InDamage);

	UFUNCTION()
	void SetCurve(UCurveFloat* InCurve);

	/** ============================ **/
	/** Public Methods: Conditionals **/

	UFUNCTION()
	bool DidPickUp(AActor* OtherActor);

	protected:
	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/
	void PlayTimeline();

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnExplode();

	/** ========================== **/
	/** Protected Methods: VFX/SFX **/

	UFUNCTION()
	void ModulateColor(const float InterpValue);

	UFUNCTION()
	void TriggerExplosionFX();

	/** ============================ **/
	/** Protected Fields: Components **/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UTimelineComponent* StickyTimelineComp;

	/** ================================== **/
	/** Protected Fields: Timeline-Members **/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UCurveFloat* StickyTimelineCurve;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

	/** ======================================= **/
	/** Private Methods: Component Initializers **/
	private:
	void ConstructCollisionComponent();
	void ConstructProjectileMovementComponent();
	void ConstructStaticMeshComponent();

	/** ================================ **/
	/** Private Fields: Basic Properties **/
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	// Projectile damage
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageValue;

	// Projectile damage-radius
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageRadius;

	float MaxPossibleLifetime = 8.0f;
	float MaxCurrentLifetime	= MaxPossibleLifetime;
};
