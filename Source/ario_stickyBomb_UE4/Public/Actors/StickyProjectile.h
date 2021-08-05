// Ario Amin - 2021/08

#pragma once

#include "Actors/StickyBaseActor.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/TimelineComponent.h>

#include "StickyProjectile.generated.h"

/**
 * @author  Ario Amin
 * @file    Actors/StickyProjectile.h
 * @class   AStickyProjectile
 * @brief   A projectile moving actor class
 * @details A projectile actor which acts as a sticky bomb. Networked, but only naively implemented.
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyProjectile : public AStickyBaseActor
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new AStickyProjectile object
	 *
	 */
	AStickyProjectile();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */
	virtual void Tick(float DeltaTime) final;
	virtual void BeginPlay() final;
	virtual void LifeSpanExpired() final;

	public:
	/* ============================== */
	/** Interface Methods: Interaction */
	virtual void TryInteractItem() final;
	virtual void EndInteractItem() final;

	/* ========================= */
	/* Public Methods: Interface */

	/**
	 * @brief Get the Max Lifetime object
	 *
	 * @return float
	 */
	float GetMaxLifetime() const;

	/**
	 * @brief Get the Collision Comp object
	 *
	 * @return USphereComponent*
	 */
	USphereComponent* GetCollisionComp() const;

	/**
	 * @brief Get the Projectile Movement object
	 *
	 * @return UProjectileMovementComponent*
	 */
	UProjectileMovementComponent* GetProjectileMovement() const;

	/**
	 * @brief Get the Timeline Comp object
	 *
	 * @return UTimelineComponent*
	 */
	UTimelineComponent* GetTimelineComp() const;

	/**
	 * @brief Get the Replicated Components object
	 *
	 * @return TArray<UActorComponent*>&
	 */
	TArray<UActorComponent*>& GetReplicatedComponents();

	/**
	 * @brief Get the Damage Radius object
	 *
	 * @return float
	 */
	UFUNCTION()
	float GetDamageRadius() const;

	/**
	 * @brief Get the Damage Amount object
	 *
	 * @return float
	 */
	UFUNCTION()
	float GetDamageAmount() const;

	/**
	 * @brief Set the Damage Radius object
	 *
	 * @param InRadius
	 */
	UFUNCTION()
	void SetDamageRadius(float InRadius);

	/**
	 * @brief Set the Damage Amount object
	 *
	 * @param InDamage
	 */
	UFUNCTION()
	void SetDamageAmount(float InDamage);

	/**
	 * @brief Set the Max Possible Lifetime object
	 *
	 * @param MaxLifetime
	 */
	UFUNCTION()
	void SetMaxPossibleLifetime(float MaxLifetime);

	/**
	 * @brief Set the Curve object
	 *
	 * @param InCurve
	 */
	UFUNCTION()
	void SetCurve(UCurveFloat* InCurve);

	/**
	 * @brief Get the Curve object
	 *
	 * @return UCurveFloat*
	 */
	UFUNCTION()
	UCurveFloat* GetCurve();

	/**
	 * @brief
	 *
	 * @param OtherActor
	 * @return true
	 * @return false
	 */
	UFUNCTION()
	bool DidPickup(AActor* OtherActor);

	protected:
	/* ======================================== */
	/* Protected Methods: Client/Server actions */

	/**
	 * @brief
	 *
	 * @param HitComp
	 * @param OtherActor
	 * @param OtherComp
	 * @param NormalImpulse
	 * @param Hit
	 */
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void OnExplode();

	/**
	 * @brief
	 *
	 * @param CallerBaseShooterActor
	 */
	UFUNCTION()
	void OnPickup(ABaseShooter* CallerBaseShooterActor);

	/* ========================== */
	/* Protected Methods: VFX/SFX */
	/**
	 * @brief Multicasted timeline callback
	 *
	 * @param InterpValue
	 */
	UFUNCTION(NetMulticast, Reliable)
	void ModulateColor(const float InterpValue);

	/**
	 * @brief
	 *
	 */
	UFUNCTION()
	void TriggerExplosionFX();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable)
	void ServerTriggerExplosionFX();

	/**
	 * @brief Multicast effects to all clients
	 *
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastTriggerExplosionFX();

	/* ============================ */
	/* Protected Fields: Components */

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UTimelineComponent* StickyTimelineComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystemComponent* ParticleSystemComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystem* ParticleFX = nullptr;

	/* ================================== */
	/* Protected Fields: Timeline-Members */

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UCurveFloat* StickyTimelineCurve = nullptr;

	// Delegate signature for the function which will handle our Finished event.
	FOnTimelineEvent TimelineFinishedEvent;

	// Delegate signature for the function which will handle our timeline loop.
	FOnTimelineFloat InterpTimelineEvent;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

	/* ======================================= */
	/* Private Methods: Component Initializers */
	private:
	/**
	 * @brief Constructs and sets up collision sphere
	 *
	 */
	void ConstructCollisionComponent();

	/**
	 * @brief Set the Collision Responses object
	 *
	 */
	void SetCollisionResponses();

	/**
	 * @brief Construct the Projectile Movement Component
	 *
	 */
	void ConstructProjectileMovementComponent();

	/**
	 * @brief Construct the Static Mesh component and set it's mesh
	 *
	 */
	void ConstructStaticMeshComponent();

	/**
	 * @brief Initializes VFX and SFX assets.
	 *
	 */
	void InitializeFXAssets();

	/* ================================ */
	/* Private Fields: Basic Properties */
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	// Explosion Sound, USoundBase
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	USoundBase* ExplosionSFX;

	// Projectile damage
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageValue = 60.0f;

	// Projectile damage-radius
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageRadius = 500.f;

	// Projectile damage-radius
	UPROPERTY(Replicated /* Using = OnRep_TryAttachToActor */, VisibleDefaultsOnly, Category = Interaction)
	ABaseShooter* AttachedToActor = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Interaction)
	AStickyGameMode* CurrentGameMode = nullptr;

	/**An array of nearby actors**/
	UPROPERTY(EditAnywhere)
	TArray<AActor*> NearbyActors;

	float MaxPossibleLifetime = 8.0f;
	float MaxCurrentLifetime	= MaxPossibleLifetime;
};
