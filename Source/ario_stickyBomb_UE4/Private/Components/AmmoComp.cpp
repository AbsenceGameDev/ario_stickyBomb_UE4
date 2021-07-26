// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AmmoComp.h"

#include "Net/UnrealNetwork.h"
#include "StickyGameMode.h"

// Sets default values for this component's properties
UAmmoComp::UAmmoComp()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}

// Called when the game starts
void UAmmoComp::BeginPlay()
{
  Super::BeginPlay();

  // ...
}

void UAmmoComp::OnReplicateAmmo(int PrevAmmo)
{
  float AmmoDiff = AmmoCount - PrevAmmo;

  OnAmmoChanged.Broadcast(this, AmmoCount, AmmoDiff);
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
  OnAmmoChanged.Broadcast(this, AmmoCount, RoundsOfAmmo);
}

// Called every frame
void UAmmoComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

int UAmmoComp::GetAmmo() const
{
  return AmmoCount;
}

bool UAmmoComp::IsEmpty() const
{
  return (AmmoCount < 1);
}

bool UAmmoComp::IsFullClip() const
{
  return AmmoCount >= MaxAmmo;
}

void UAmmoComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(UAmmoComp, AmmoCount);
}