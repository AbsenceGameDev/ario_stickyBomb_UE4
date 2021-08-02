// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Interfaces/InteractionUOI.h"

#include <Components/TimelineComponent.h>
#include <GameFramework/Actor.h>

#include "StickyProjectile.generated.h"

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyProjectile : public AActor, public IInteractionUOI
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
	/** ============================== **/
	/** Interface Methods: Interaction **/
	virtual void TryInteractItem() override;
	virtual void EndInteractItem() override;

	/** =============================== **/
	/** Public Methods: Getters/Setters **/
	float													GetMaxLifetime() const;
	USphereComponent*							GetCollisionComp() const;
	UProjectileMovementComponent* GetProjectileMovement() const;
	UTimelineComponent*						GetTimelineComp() const;
	TArray<UActorComponent*>&			GetReplicatedComponents();

	UFUNCTION()
	float GetDamageRadius() const;

	UFUNCTION()
	float GetDamageAmount() const;

	UFUNCTION()
	void SetDamageRadius(float InRadius);

	UFUNCTION()
	void SetDamageAmount(float InDamage);

	UFUNCTION()
	void SetMaxPossibleLifetime(float MaxLifetime);

	UFUNCTION()
	void SetCurve(UCurveFloat* InCurve);

	UFUNCTION()
	UCurveFloat* GetCurve();

	/** ============================ **/
	/** Public Methods: Conditionals **/

	UFUNCTION()
	bool DidPickup(AActor* OtherActor);

	protected:
	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/
	void PlayTimeline();

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// UFUNCTION(NetMulticast, Reliable)
	// void MulticastAttachToPlayer(int32 LocalPlayerId);
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerFetchPlayer(int32 LocalPlayerId);

	UFUNCTION()
	void OnExplode();

	UFUNCTION()
	void OnPickup(ABaseShooter* CallerBaseShooterActor);

	// UFUNCTION()
	// void OnRep_TryAttachToActor(ABaseShooter* TryAttachActor);

	/** ========================== **/
	/** Protected Methods: VFX/SFX **/

	UFUNCTION()
	void ModulateColor(const float InterpValue);

	UFUNCTION()
	void TriggerExplosionFX();

	/** ============================ **/
	/** Protected Fields: Components **/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UTimelineComponent* StickyTimelineComp = nullptr;

	/** ================================== **/
	/** Protected Fields: Timeline-Members **/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UCurveFloat* StickyTimelineCurve = nullptr;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

	/** ======================================= **/
	/** Private Methods: Component Initializers **/
	private:
	void ConstructCollisionComponent();
	void SetCollisionResponses();
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

	// Projectile damage-radius
	UPROPERTY(Replicated /* Using = OnRep_TryAttachToActor */, VisibleDefaultsOnly, Category = Interaction)
	ABaseShooter* AttachedToActor = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Interaction)
	AStickyGameMode* CurrentGameMode = nullptr;

	float MaxPossibleLifetime = 8.0f;
	float MaxCurrentLifetime	= MaxPossibleLifetime;
};
