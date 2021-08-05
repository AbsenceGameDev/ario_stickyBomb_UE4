/**
 * @author  Ario Amin
 * @file    Components/HealthComp.cpp
 * @class   UHealthComp
 * @brief   Networked health component
 * @details Simple networked health component implementation, to be used with ABaseShooter derived characters,
 *          but could be retrofitted for other character types if it is rewritten as a templated class
 **/
#include "Components/HealthComp.h"

// Engine Classes/Types
#include "StickyGameMode.h"

// Helpers
#include "Helpers/Macros.h"

// Engine Frameworks
#include <GameFramework/Actor.h>
#include <Math/UnrealMathUtility.h>
#include <Net/UnrealNetwork.h>

// Sets default values for this component's properties
UHealthComp::UHealthComp()
{
	MaxHealth = 100.0f;
	bIsDead		= false;

	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void UHealthComp::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority) {
		AActor* MyOwner = GetOwner();
		if (MyOwner) {
			// Rifle Damage = onTakePointDamage delegate -> HandleDamageHit -> Blueprint / C++ Update UI
			MyOwner->OnTakePointDamage.AddDynamic(this, &UHealthComp::HandleDamageHit);
			// Grenade Damage
			MyOwner->OnTakeRadialDamage.AddDynamic(this, &UHealthComp::HandleRadialDamage);
		}
	}
	Health = MaxHealth;
}

/** ================================ **/
/** Public Methods: Client interface **/
void UHealthComp::TryHeal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f) {
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));
#endif		// STICKY_DEBUG
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void UHealthComp::TryTakeDamage(
	AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser)
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("Try Damage Health: -%sHP"), *FString::SanitizeFloat(DamageAmount));
#endif		// STICKY_DEBUG

	if (DamageAmount <= 0.0f || Health <= 0.0f || (GetOwnerRole() < ROLE_Authority)) {
		return;
	}
	HandleTakeAnyDamage(DamagedActor, DamageAmount, DamageType, EventInstigator, DamageCauser);
}

/** ======================================== **/
/** Protected Methods: Client/Server actions **/
void UHealthComp::OnRep_Health(float PrevHealth)
{
	float Damage = Health - PrevHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHealthComp::HandleTakeAnyDamage(
	AActor* DamagedActor, float DamageAmount, const class UDamageType* DamageType, class AController* InstigatedBy,
	AActor* DamageCauser)
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("Try Damage Health 2nd Pass: -%sHP"), *FString::SanitizeFloat(DamageAmount));
#endif		// STICKY_DEBUG
	if (DamageAmount <= 0.0f || bIsDead || (DamageCauser == DamagedActor)) {
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
#endif		// STICKY_DEBUG
	bIsDead = Health <= 0.0f;

	if (bIsDead) {
		AStickyGameMode* GM = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) {
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
	OnHealthChanged.Broadcast(this, Health, DamageAmount, DamageType, InstigatedBy, DamageCauser);
}

void UHealthComp::HandleRadialDamage(
	AActor* DamagedActor, float DamageAmount, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || bIsDead) {
		return;
	}

	AController* InstigatorController =
		DamageCauser && DamageCauser->GetOwner() != nullptr ? DamageCauser->GetOwner()->GetInstigatorController() : nullptr;

	if (DamageCauser && DamageCauser != DamagedActor) {
		return;
	}

	Health	= FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	bIsDead = Health <= 0.0f;
	OnHealthChanged.Broadcast(this, Health, DamageAmount, DamageType, InstigatedBy, DamageCauser);

	/* Call will only succeed on the server */
	AStickyGameMode* CurrentGameMode = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());

	// Projectile -> Grenade Weapon -> Actor Owner
	if (bIsDead && CurrentGameMode && InstigatorController)
		CurrentGameMode->OnActorKilled.Broadcast(DamageCauser, GetOwner(), InstigatorController);
}

void UHealthComp::HandleDamageHit(
	AActor* DamagedActor, float DamageAmount, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent,
	FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || bIsDead || DamageCauser == DamagedActor) {
		return;
	}

	Health	= FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	bIsDead = Health <= 0.0f;
	OnHealthChanged.Broadcast(this, Health, DamageAmount, DamageType, InstigatedBy, DamageCauser);

	/* Call will only succeed on the server */
	AStickyGameMode* CurrentGameMode = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());

	if (CurrentGameMode) {
		CurrentGameMode->OnHitEvent.Broadcast(HitLocation, ShotFromDirection, DamageAmount, GetOwner());
	}

	if (bIsDead && CurrentGameMode) {
		CurrentGameMode->OnActorKilled.Broadcast(DamageCauser, GetOwner(), InstigatedBy);
	}
}

/** =========================== **/
/** Static Methods: Replication **/
void UHealthComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComp, Health);
}
