// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StickyGunSkeletalComp.h"

#include "Actors/StickyProjectile.h"

void UStickyGunSkeletalComp::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr) {
	  UWorld* const World = GetWorld();
		if (World != nullptr) {
		  const FRotator SpawnRotation = OwningCharacter->GetControlRotation();
		  // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to
		  // find the final muzzle position
		  const FVector SpawnLocation =
			((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : OwningCharacter->GetActorLocation()) +
			SpawnRotation.RotateVector(GunOffset);

		  // Set Spawn Collision Handling Override
		  FActorSpawnParameters ActorSpawnParams;
		  ActorSpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		  // spawn the projectile at the muzzle
		  World->SpawnActor<AStickyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		  AmmoComp->ChangeAmmoCount(-1);
		  OwningCharacter->FireGunEffects(this);
		}
	}
}

void UStickyGunSkeletalComp::InitStickyGun(USceneComponent* LocalRootComponent, FVector LocalGunOffset)
{
  SetOnlyOwnerSee(false);	 // otherwise won't be visible in the multiplayer
  bCastDynamicShadow = false;
  CastShadow = false;
  SetupAttachment(LocalRootComponent);

  // FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));	  // causes crash
  // FP_MuzzleLocation->SetupAttachment(this);
  // FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
  GunOffset = LocalGunOffset;
}

UAmmoComp* UStickyGunSkeletalComp::GetAmmoCompPtr()
{
  return AmmoComp;
}