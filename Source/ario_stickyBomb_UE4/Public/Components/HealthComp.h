// Fill out your copyright notice in the Description page of Project Settings.

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UHealthComp : public UActorComponent
{
	GENERATED_BODY()

	public:
	// Sets default values for this component's properties
	UHealthComp();

	protected:
	/** ============================ **/
	/** Inherited Methods: Overrides **/
	virtual void BeginPlay() override;

	public:
	/** ================================ **/
	/** Public Methods: Client interface **/
	UFUNCTION(BlueprintCallable, Category = "Heal")
	void TryHeal(float HealAmount);

	/** ================================ **/
	/** Public Methods: Inline Getters **/
	float GetHealth() const { return Health; }
	bool	IsDead() const { return bIsDead; }

	/** ================================ **/
	/** Public Fields: Events/Delegates **/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	protected:
	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/
	UFUNCTION()
	void OnRep_Health(float PrevHealth);

	UFUNCTION()
	void HandleTakeAnyDamage(
		AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleRadialDamage(
		AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
		AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleDamageHit(
		AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent,
		FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	/** ================================== **/
	/** Protected Fields: Basic Properties **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health = MaxHealth;
};
