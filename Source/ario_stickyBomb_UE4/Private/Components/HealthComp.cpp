// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/HealthComp.h"
#include "Net/UnrealNetwork.h"
#include "StickyGameMode.h"

// Sets default values for this component's properties
UHealthComp::UHealthComp() {
  MaxHealth = 100;
  bIsDead = false;

  SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UHealthComp::BeginPlay() {
  Super::BeginPlay();

  // Only hook if we are server
  if (GetOwnerRole() == ROLE_Authority) {
    AActor *MyOwner = GetOwner();
    if (MyOwner) {
      MyOwner->OnTakeAnyDamage.AddDynamic(this,
                                          &UHealthComp::HandleTakeAnyDamage);
    }
  }

  Health = MaxHealth;
}

void UHealthComp::OnReplicateHealth(float PrevHealth) {
  float Damage = Health - PrevHealth;

  OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHealthComp::HandleTakeAnyDamage(AActor *DamagedActor, float Damage,
                                      const class UDamageType *DamageType,
                                      class AController *InstigatedBy,
                                      AActor *DamageCauser) {
  if (Damage <= 0.0f || bIsDead || (DamageCauser != DamagedActor)) {
    return;
  }

  // Update health clamped
  Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

  UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"),
         *FString::SanitizeFloat(Health));

  bIsDead = Health <= 0.0f;

  OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy,
                            DamageCauser);

  if (bIsDead) {
    AStickyGameMode *GM = Cast<AStickyGameMode>(GetWorld()->GetAuthGameMode());
    if (GM) {
      GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
    }
  }
}

void UHealthComp::Regenerate(float RegenAmount) {
  if (RegenAmount <= 0.0f || Health <= 0.0f) {
    return;
  }

  Health = FMath::Clamp(Health + RegenAmount, 0.0f, MaxHealth);

  UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"),
         *FString::SanitizeFloat(Health), *FString::SanitizeFloat(RegenAmount));

  OnHealthChanged.Broadcast(this, Health, -RegenAmount, nullptr, nullptr,
                            nullptr);
}

float UHealthComp::GetHealth() const { return Health; }

void UHealthComp::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const {
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(UHealthComp, Health);
}
