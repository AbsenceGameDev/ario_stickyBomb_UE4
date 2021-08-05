// Ario Amin - 2021/08

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Engine/EngineTypes.h>

#include "HealthComp.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(
	FOnHealthChangedSignature, UHealthComp*, OwningHealthComp, float, Health, float, HealthDelta, const UDamageType*, DamageType,
	AController*, InstigatedBy, AActor*, DamageCauser);

/**
 * @author  Ario Amin
 * @file    Components/HealthComp.h
 * @class   UHealthComp
 * @brief   Networked health component
 * @details Simple networked health component, to be used with ABaseShooter derived characters,
 *          but could be retrofitted for other character types if it is rewritten as a templated class
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UHealthComp : public UActorComponent
{
	GENERATED_BODY()

	public:
	UHealthComp();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief   Component Begin Play
	 *
	 */
	virtual void BeginPlay() override;

	public:
	/* ================================ */
	/* Public Methods: Client interface */

	/**
	 * @brief   Send Heal request, server decides if it is valid
	 *
	 * @param   HealAmount
	 */
	UFUNCTION(BlueprintCallable, Category = "Heal")
	void TryHeal(float HealAmount);

	/**
	 * @brief   Send Damage request, server decides if it is valid
	 *
	 * @param   DamagedActor
	 * @param   DamageAmount
	 * @param   DamageType
	 * @param   EventInstigator
	 * @param   DamageCauser
	 */
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TryTakeDamage(
		AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser);

	/* ================================ */
	/* Public Methods: Inline Getters */

	/**
	 * @brief   Get the Health object
	 *
	 * @return  float
	 */
	float GetHealth() const { return Health; }

	/**
	 * @brief   Check if Component bIsDead flag is set
	 *
	 * @return  true | false
	 */
	bool IsDead() const { return bIsDead; }

	/* ================================ */
	/* Public Fields: Events/Delegates */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	protected:
	/* ======================================== */
	/* Protected Methods: Client/Server actions */

	/**
	 * @brief   OnReplicate health
	 *
	 * @param   PrevHealth
	 */
	UFUNCTION()
	void OnRep_Health(float PrevHealth);

	/**
	 * @brief   TakeAnyDamage event
	 *
	 * @param   DamagedActor
	 * @param   DamageAmount
	 * @param   DamageType
	 * @param   InstigatedBy
	 * @param   DamageCauser
	 */
	UFUNCTION()
	void HandleTakeAnyDamage(
		AActor* DamagedActor, float DamageAmount, const class UDamageType* DamageType, class AController* InstigatedBy,
		AActor* DamageCauser);

	/**
	 * @brief   TakeRadialDamage event
	 *
	 * @param   DamagedActor
	 * @param   DamageAmount
	 * @param   DamageType
	 * @param   Origin
	 * @param   HitInfo
	 * @param   InstigatedBy
	 * @param   DamageCauser
	 */
	UFUNCTION()
	void HandleRadialDamage(
		AActor* DamagedActor, float DamageAmount, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
		AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * @brief   PointDamage Hit event
	 *
	 * @param   DamagedActor
	 * @param   DamageAmount
	 * @param   InstigatedBy
	 * @param   HitLocation
	 * @param   FHitComponent
	 * @param   BoneName
	 * @param   ShotFromDirection
	 * @param   DamageType
	 * @param   DamageCauser
	 */
	UFUNCTION()
	void HandleDamageHit(
		AActor* DamagedActor, float DamageAmount, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent,
		FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	/* ================================== */
	/* Protected Fields: Basic Properties */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health = MaxHealth;
};
