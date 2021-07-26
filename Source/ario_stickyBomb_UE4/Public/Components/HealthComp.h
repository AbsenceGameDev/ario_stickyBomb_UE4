// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

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

  UPROPERTY(ReplicatedUsing = OnReplicateHealth, BlueprintReadOnly, Category = "HealthComp")
  float Health = 100.0f;

  UFUNCTION()
  void OnReplicateHealth(float PrevHealth);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComp")
  float MaxHealth;

  UFUNCTION()
  void HandleTakeAnyDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

  public:
  float GetHealth() const
  {
	return Health;
  }

  bool IsDead() const
  {
	return bIsDead;
  }

  UPROPERTY(BlueprintAssignable, Category = "Events")
  FOnHealthChangedSignature OnHealthChanged;

  UFUNCTION(BlueprintCallable, Category = "HealthComp")
  void Regenerate(float RegenAmount);
};
