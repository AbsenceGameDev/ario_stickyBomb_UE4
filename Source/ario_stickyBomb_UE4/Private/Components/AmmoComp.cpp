// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AmmoComp.h"

#include "StickyGameMode.h"

#include <Net/UnrealNetwork.h>


// Sets default values for this component's properties
UAmmoComp::UAmmoComp()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  MaxAmmo = 100.0f;
  bIsEmpty = false;

  SetIsReplicatedByDefault(true);
  SetIsReplicated(true);
}

// Called when the game starts
void UAmmoComp::BeginPlay()
{
  Super::BeginPlay();

  AmmoCount = MaxAmmo;
}

void UAmmoComp::OnRep_Ammo(int PrevAmmo)
{
  float AmmoDiff = AmmoCount - PrevAmmo;
  OnAmmoChanged.Broadcast(this, AmmoCount, AmmoDiff, nullptr);
}

void UAmmoComp::ChangeAmmoCount(int RoundsOfAmmo)
{
  int RoundsAbs = (-RoundsOfAmmo * (RoundsOfAmmo < 0)) + (RoundsOfAmmo * (RoundsOfAmmo > 0));
	// Not enough rounds left in the clip or called with RoundsOfAmmo = 0
	if (RoundsOfAmmo == 0 || bIsEmpty || RoundsAbs > AmmoCount) {
	  return;
	}

  AmmoCount += RoundsOfAmmo;
  bIsEmpty = AmmoCount < 1;

  UE_LOG(LogTemp, Log, TEXT("Ammo Changed: %s"), *FString::FromInt(AmmoCount));
  OnAmmoChanged.Broadcast(this, AmmoCount, RoundsOfAmmo, nullptr);
}

// Called every frame
void UAmmoComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

void UAmmoComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(UAmmoComp, AmmoCount);
}