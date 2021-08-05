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
	 * @brief   Construct a new AStickyProjectile object
	 *
	 */
	AStickyProjectile();

	protected:
	/* ============================ */
	/* Inherited Methods: Overrides */
	virtual void Tick(float DeltaTime) final;
	virtual void BeginPlay() final;
	virtual void LifeSpanExpired() final;

	public:
	/* ============================== */
	/* Interface Methods: Interaction */
	virtual void TryInteractItem() final;
	virtual void EndInteractItem() final;

	/* ========================= */
	/* Public Methods: Interface */

	/**
	 * @brief   Get the Max Lifetime
	 *
	 * @return  float
	 */
	float GetMaxLifetime() const;

	/**
	 * @brief   Get the Collision Comp object
	 *
	 * @return  USphereComponent*
	 */
	USphereComponent* GetCollisionComp() const;

	/**
	 * @brief   Get the Projectile Movement object
	 *
	 * @return  UProjectileMovementComponent*
	 */
	UProjectileMovementComponent* GetProjectileMovement() const;

	/**
	 * @brief   Get the Timeline Comp object
	 *
	 * @return  UTimelineComponent*
	 */
	UTimelineComponent* GetTimelineComp() const;

	/**
	 * @brief   Get the Replicated Components object
	 *
	 * @return  TArray<UActorComponent*>&
	 */
	TArray<UActorComponent*>& GetReplicatedComponents();

	/**
	 * @brief   Get the Damage Radius
	 *
	 * @return  float
	 */
	UFUNCTION()
	float GetDamageRadius() const;

	/**
	 * @brief   Get the Damage Amount
	 *
	 * @return  float
	 */
	UFUNCTION()
	float GetDamageAmount() const;

	/**
	 * @brief   Set the Damage Radius
	 *
	 * @param   InRadius
	 */
	UFUNCTION()
	void SetDamageRadius(float InRadius);

	/**
	 * @brief   Set the Damage Amount
	 *
	 * @param   InDamage
	 */
	UFUNCTION()
	void SetDamageAmount(float InDamage);

	/**
	 * @brief   Set the Max Possible Lifetime length
	 *
	 * @param   MaxLifetime
	 */
	UFUNCTION()
	void SetMaxPossibleLifetime(float MaxLifetime);

	/**
	 * @brief   Set the Curve object
	 *
	 * @param   InCurve
	 */
	UFUNCTION()
	void SetCurve(UCurveFloat* InCurve);

	/**
	 * @brief   Get the Curve object
	 *
	 * @return  UCurveFloat*
	 */
	UFUNCTION()
	UCurveFloat* GetCurve();

	/**
	 * @brief   Tries picking up actor, returns to reflect if it was successful
	 *
	 * @param   OtherActor
	 * @return  true | false
	 */
	UFUNCTION()
	bool DidPickup(AActor* OtherActor);

	protected:
	/* ======================================== */
	/* Protected Methods: Client/Server actions */

	/**
	 * @brief   Called when a hit triggers the collision of the actor.
	 *
	 * @param   HitComp
	 * @param   OtherActor
	 * @param   OtherComp
	 * @param   NormalImpulse
	 * @param   Hit
	 */
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * @brief   Decides what happens when the projectile explodes
	 * @details If server accepts, send radial damage and replicate it and then multicast VFX/SFX to clients
	 *
	 */
	UFUNCTION()
	void OnExplode();

	/* ========================== */
	/* Protected Methods: VFX/SFX */
	/**
	 * @brief   Multicasted timeline callback
	 *
	 * @param   InterpValue
	 */
	UFUNCTION(NetMulticast, Reliable)
	void ModulateColor(const float InterpValue);

	/**
	 * @brief   Triggers explosion effects from the ue4 starter content
	 *
	 */
	UFUNCTION()
	void TriggerExplosionFX();

	/**
	 * @brief   Server check
	 *
	 */
	UFUNCTION(Server, Reliable)
	void ServerTriggerExplosionFX();

	/**
	 * @brief   Multicast TriggerExplosionFX call to all clients
	 *
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastTriggerExplosionFX();

	/* ============================ */
	/* Protected Fields: Components */

	/** @brief  Used as a primitive collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp = nullptr;

	/** @brief  Handles projectile movement with an easy to use interface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	/** @brief  Mesh component of the projectile */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr = nullptr;

	/** @brief  Timeline component to be used to modulate material color */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UTimelineComponent* StickyTimelineComp = nullptr;

	/** @brief  Particle Systems Component to spawn particle effects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystemComponent* ParticleSystemComp = nullptr;

	/** @brief  Particle System Type to actually hold the particle effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystem* ParticleFX = nullptr;

	/* ================================== */
	/* Protected Fields: Timeline-Members */

	/** @brief  UCurve object to store the UCurve that is generated by StickyGunSkeletalComp */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UCurveFloat* StickyTimelineCurve = nullptr;

	/** @brief  Delegate signature for the function which will handle our Finished event. */
	FOnTimelineEvent TimelineFinishedEvent;

	/** @brief  Delegate signature for the function which will handle our timeline loop. */
	FOnTimelineFloat InterpTimelineEvent;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

	/* ======================================= */
	/* Private Methods: Component Initializers */
	private:
	/**
	 * @brief   Constructs and sets up collision sphere
	 *
	 */
	void ConstructCollisionComponent();

	/**
	 * @brief   Set the Collision Responses object
	 *
	 */
	void SetCollisionResponses();

	/**
	 * @brief   Construct the Projectile Movement Component
	 *
	 */
	void ConstructProjectileMovementComponent();

	/**
	 * @brief   Construct the Static Mesh component and set it's mesh
	 *
	 */
	void ConstructStaticMeshComponent();

	/**
	 * @brief   Initializes VFX and SFX assets.
	 *
	 */
	void InitializeFXAssets();

	/* ================================ */
	/* Private Fields: Basic Properties */
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	/** @brief  Projectile material */
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	/** @brief  Explosion Sound, USoundBase */
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	USoundBase* ExplosionSFX;

	/** @brief  Projectile damage */
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageValue = 60.0f;

	/** @brief  Projectile damage-radius */
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageRadius = 500.f;

	/** @brief  Replicated actor of which it is attached to */
	UPROPERTY(Replicated /* Using = OnRep_TryAttachToActor */, VisibleDefaultsOnly, Category = Interaction)
	ABaseShooter* AttachedToActor = nullptr;

	/** @brief  GameMode class which is currently active */
	UPROPERTY(VisibleDefaultsOnly, Category = Interaction)
	AStickyGameMode* CurrentGameMode = nullptr;

	/** @brief  An array of nearby actors */
	UPROPERTY(EditAnywhere)
	TArray<AActor*> NearbyActors;

	float MaxPossibleLifetime = 8.0f;
	float MaxCurrentLifetime	= MaxPossibleLifetime;
};
