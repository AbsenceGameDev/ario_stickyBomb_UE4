// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Interfaces/InteractionUOI.h"

#include <Components/TimelineComponent.h>
#include <GameFramework/Actor.h>
#include <Templates/SharedPointer.h>

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

	/** ========================== **/
	/** Protected Methods: VFX/SFX **/
	UFUNCTION(NetMulticast, Reliable)
	void ModulateColor(const float InterpValue);

	UFUNCTION()
	void TriggerExplosionFX();

	UFUNCTION(Server, Reliable)
	void ServerTriggerExplosionFX();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastTriggerExplosionFX();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystemComponent* ParticleSystemComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ParticleEffect")
	UParticleSystem* ParticleFX = nullptr;

	/** ================================== **/
	/** Protected Fields: Timeline-Members **/

	// UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	// TSharedPtr<UCurveFloat> StickyTimelineCurve = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UCurveFloat* StickyTimelineCurve = nullptr;

	// Delegate signature for the function which will handle our Finished event.
	FOnTimelineEvent TimelineFinishedEvent;

	// Delegate signature for the function which will handle our timeline loop.
	FOnTimelineFloat InterpTimelineEvent;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection = ETimelineDirection::Type::Forward;

	/** ======================================= **/
	/** Private Methods: Component Initializers **/
	private:
	void ConstructCollisionComponent();
	void SetCollisionResponses();
	void ConstructProjectileMovementComponent();
	void ConstructStaticMeshComponent();
	void InitializeFXAssets();

	/** ================================ **/
	/** Private Fields: Basic Properties **/
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	// Explosion Sound, USoundBase
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	USoundBase* ExplosionSFX;

	// Projectile damage
	UPROPERTY(VisibleDefaultsOnly, Category = Damage)
	float DamageValue = 50.0f;

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
