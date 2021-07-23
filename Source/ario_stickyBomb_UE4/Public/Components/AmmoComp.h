// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AmmoComp.generated.h"

// OnAmmoChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoChangedSignature, UAmmoComp*, OwningAmmoComp, int, Ammo, int, AmmoDelta);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UAmmoComp : public UActorComponent {
  GENERATED_BODY()

  public:
  // Sets default values for this component's properties
  UAmmoComp();

  protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  UPROPERTY(ReplicatedUsing = OnReplicateAmmo, BlueprintReadOnly, Category = "AmmoComp")
  int AmmoCount;

  UFUNCTION()
  void OnReplicateAmmo(int PrevAmmo);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoComp")
  int MaxAmmo;

  bool bIsEmpty;

  UFUNCTION()
  void HandleAmmoUsage(UAmmoComp* ThisAmmoClip, int RoundsOfAmmo);

  public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  int GetAmmo() const;

  UPROPERTY(BlueprintAssignable, Category = "Events")
  FOnAmmoChangedSignature OnAmmoChanged;
};
