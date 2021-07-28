// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include <Engine/EngineTypes.h>

#include "AmmoComp.generated.h"

// OnAmmoChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
  FOnAmmoChangedSignature, UAmmoComp*, OwningAmmoComp, int, Ammo, int, AmmoDelta, class AController*, InstigatedBy);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UAmmoComp : public UActorComponent
{
  GENERATED_BODY()

  public:
  // Sets default values for this component's properties
  UAmmoComp();

  protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  bool bIsEmpty;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoComp")
  int MaxAmmo;

  UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "AmmoComp")
  int AmmoCount;

  UFUNCTION()
  void OnRep_Ammo(int PrevAmmo);

  public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION()
  void ChangeAmmoCount(int RoundsOfAmmo);

  UPROPERTY(BlueprintAssignable, Category = "Events")
  FOnAmmoChangedSignature OnAmmoChanged;

  float GetAmmo() const
  {
	return AmmoCount;
  }
  bool IsEmpty() const
  {
	return bIsEmpty;
  }
  bool IsFullClip() const
  {
	return AmmoCount >= MaxAmmo;
  }
};
