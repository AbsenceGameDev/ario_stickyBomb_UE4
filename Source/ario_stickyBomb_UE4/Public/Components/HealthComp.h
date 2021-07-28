// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include <Engine/EngineTypes.h>

#include "HealthComp.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComp*, OwningHealthComp, float, Health, float,
  HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UHealthComp : public UActorComponent
{
  GENERATED_BODY()

  public:
  // Sets default values for this component's properties
  UHealthComp();

  protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  bool bIsDead = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComp")
  float MaxHealth = 100.0f;

  UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
  float Health = MaxHealth;

  UFUNCTION()
  void OnRep_Health(float PrevHealth);

  UFUNCTION()
  void HandleTakeAnyDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

  UFUNCTION()
  void HandleRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin,
	FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

  UFUNCTION()
  void HandleDamageHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
	class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType,
	AActor* DamageCauser);

  public:
  UPROPERTY(BlueprintAssignable, Category = "Events")
  FOnHealthChangedSignature OnHealthChanged;

  // less memory 0 - 255 \ doesn't need to be replicated, default team we spawn with
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
  uint8 TeamNumber;

  // no white execution pin = blueprint Pure
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
  static bool IsFriendly(AActor* left, AActor* right);

  UFUNCTION(BlueprintCallable, Category = "Heal")
  void Heal(float HealAmount);

  float GetHealth() const
  {
	return Health;
  }

  bool IsDead() const
  {
	return bIsDead;
  }
};
