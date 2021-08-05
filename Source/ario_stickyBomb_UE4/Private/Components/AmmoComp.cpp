/**
 * @author  Ario Amin
 * @file    Components/AmmoComp.cpp
 * @class   UAmmoComp
 * @brief   Simple ammo component.
 * @details A simple but networked ammo component.
 * @todo    This component could be expaned into something more like an inventory.
 *          What would be needed is making a dedicated item_data class, hold the item_data in the inventory.
 *          Then spawn the items on server-side when brought out of inventory.
 */
#include "Components/AmmoComp.h"

#include "Characters/BaseShooter.h"
#include "StickyGameMode.h"

// Helpers
#include "Helpers/Macros.h"

// Engine Frameworks
#include <Net/UnrealNetwork.h>

// Sets default values for this component's properties
UAmmoComp::UAmmoComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	AmmoCount = MaxAmmo;
	bIsEmpty	= false;

	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

/* ============================ */
/** Inherited Methods: Overrides */

void UAmmoComp::BeginPlay() { Super::BeginPlay(); }

// Disabled
void UAmmoComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/* ================================ */
/* Public Methods: Client interface */

void UAmmoComp::TryFire() { OnFire(); }
void UAmmoComp::TryPickupRound() { OnPickupRound(); }

/* ======================================== */
/* Protected Methods: Client/Server actions */

void UAmmoComp::OnRep_Ammo(int PrevAmmo)
{
	float AmmoDiff = AmmoCount - PrevAmmo;
	OnAmmoChanged.Broadcast(this, AmmoCount, AmmoDiff, nullptr);
}

void UAmmoComp::OnFire()
{
	if (GetOwnerRole() < ROLE_Authority) {
		return;
	}

	if (AmmoCount == 0) {
		return;
	}

	AmmoCount--;
	bIsEmpty = (AmmoCount = LL_CLAMP(AmmoCount, 0, MaxAmmo)) < 1;

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
	StaticCast<ABaseShooter*>(GetOwner())->TriggerPlayerStateAmmo(AmmoCount);

	// OnAmmoChanged.Broadcast(this, AmmoCount, RoundsOfAmmo, nullptr);
}

void UAmmoComp::ServerOnPickupRound_Implementation() { OnPickupRound(); }
bool UAmmoComp::ServerOnPickupRound_Validate() { return true; }
void UAmmoComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAmmoComp, AmmoCount);
}