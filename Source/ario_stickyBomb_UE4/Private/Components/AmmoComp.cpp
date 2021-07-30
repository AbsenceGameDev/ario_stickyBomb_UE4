// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AmmoComp.h"

#include "Helpers/Macros.h"
#include "StickyGameMode.h"

#include <Net/UnrealNetwork.h>

// Sets default values for this component's properties
UAmmoComp::UAmmoComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxAmmo	 = 3;
	bIsEmpty = false;

	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void UAmmoComp::BeginPlay()
{
	Super::BeginPlay();
	AmmoCount = MaxAmmo;
}

// Disabled
void UAmmoComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/** ================================ **/
/** Public Methods: Client interface **/

void UAmmoComp::TryFire() { OnFire(); }

void UAmmoComp::TryPickupRound() { OnPickupRound(); }

/** ======================================== **/
/** Protected Methods: Client/Server actions **/

void UAmmoComp::OnRep_Ammo(int PrevAmmo)
{
	float AmmoDiff = AmmoCount - PrevAmmo;
	OnAmmoChanged.Broadcast(this, AmmoCount, AmmoDiff, nullptr);
}

void UAmmoComp::OnFire()
{
	UE_LOG(LogTemp, Log, TEXT("UAmmoComp::OnFire  Begin"));
	if (GetOwnerRole() < ROLE_Authority) {
		UE_LOG(LogTemp, Log, TEXT("UAmmoComp::OwnerCheck  Failed"));
		return;
	}

	if (AmmoCount == 0) {
		return;
	}

	AmmoCount--;
	bIsEmpty = (AmmoCount = LL_CLAMP(AmmoCount, 0, MaxAmmo)) < 1;

	UE_LOG(LogTemp, Log, TEXT("Current Ammo: %s"), *FString::FromInt(AmmoCount));
	// OnAmmoChanged.Broadcast(this, AmmoCount, RoundsOfAmmo, nullptr);
}

void UAmmoComp::ServerOnFire_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority) {
		OnFire();
	}
}

// intended for anti-cheat, validates code
bool UAmmoComp::ServerOnFire_Validate() { return true; }

void UAmmoComp::OnPickupRound()
{
	if (GetOwnerRole() < ROLE_Authority) {
		/** @todo Change validation to come from an async callback, this is not feasible having to wait for the server */
		ServerOnPickupRound();
		return;
	}

	if (AmmoCount == MaxAmmo) {
		return;
	}

	AmmoCount++;
	bIsEmpty = (AmmoCount = LL_CLAMP(AmmoCount, 0, MaxAmmo)) < 1;

	UE_LOG(LogTemp, Log, TEXT("Current Ammo: %s"), *FString::FromInt(AmmoCount));
	// OnAmmoChanged.Broadcast(this, AmmoCount, RoundsOfAmmo, nullptr);
}

void UAmmoComp::ServerOnPickupRound_Implementation() { OnPickupRound(); }

bool UAmmoComp::ServerOnPickupRound_Validate() { return true; }

void UAmmoComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAmmoComp, AmmoCount);
}