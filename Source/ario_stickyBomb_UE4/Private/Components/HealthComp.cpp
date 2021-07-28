// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/HealthComp.h"

#include "StickyGameMode.h"

#include <GameFramework/Actor.h>
#include <Math/UnrealMathUtility.h>
#include <Net/UnrealNetwork.h>


// Sets default values for this component's properties
UHealthComp::UHealthComp()
{
  MaxHealth = 100.0f;
  bIsDead = false;
  TeamNumber = 255;

  SetIsReplicatedByDefault(true);
  SetIsReplicated(true);
}

// Called when the game starts
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

bool UHealthComp::IsFriendly(AActor* LeftActor, AActor* RightActor)
{
  if (LeftActor == nullptr || RightActor == nullptr)
	return false;

  UHealthComp* HealthLeft = Cast<UHealthComp>(LeftActor->GetComponentByClass(UHealthComp::StaticClass()));
  UHealthComp* HealthRight = Cast<UHealthComp>(RightActor->GetComponentByClass(UHealthComp::StaticClass()));

  if (HealthLeft == nullptr || HealthRight == nullptr)
	return true;

  if (HealthLeft->TeamNumber == HealthRight->TeamNumber)
	return true;

  return false;
}

void UHealthComp::OnRep_Health(float PrevHealth)
{
  float Damage = Health - PrevHealth;
  OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHealthComp::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f) {
	  return;
	}

  Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
  UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));
  OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void UHealthComp::HandleTakeAnyDamage(
  AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead || (DamageCauser != DamagedActor)) {
	  return;
	}

  // Update health clamped
  Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
  UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
  bIsDead = Health <= 0.0f;

	if (bIsDead) {
	  AStickyGameMode* GM = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) {
		  GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
  OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UHealthComp::HandleRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin,
  FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser)
{
  if (Damage <= 0.0f || bIsDead)
	return;

  AController* InstigatorController =
	DamageCauser && DamageCauser->GetOwner() != nullptr ? DamageCauser->GetOwner()->GetInstigatorController() : nullptr;

  if (DamageCauser && DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser->GetOwner()))
	return;

  Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
  bIsDead = Health <= 0.0f;
  OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

  /* Call will only succeed on the server */
  AStickyGameMode* CurrentGameMode = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());

  // Projectile -> Grenade Weapon -> Actor Owner
  if (bIsDead && CurrentGameMode && InstigatorController)
	CurrentGameMode->OnActorKilled.Broadcast(DamageCauser, GetOwner(), InstigatorController);
}

void UHealthComp::HandleDamageHit(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation,
  UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType,
  AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead || DamageCauser == DamagedActor) {
	  return;
	}
	if (IsFriendly(DamagedActor, DamageCauser)) {
	  return;
	}

  Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
  bIsDead = Health <= 0.0f;
  OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

  /* Call will only succeed on the server */
  AStickyGameMode* CurrentGameMode = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());

	if (CurrentGameMode) {
	  CurrentGameMode->OnHitEvent.Broadcast(HitLocation, ShotFromDirection, Damage, GetOwner());
	}

	if (bIsDead && CurrentGameMode) {
	  CurrentGameMode->OnActorKilled.Broadcast(DamageCauser, GetOwner(), InstigatedBy);
	}
}

void UHealthComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(UHealthComp, Health);
}
