// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/StickyProjectile.h"

#include "Characters/BaseShooter.h"
#include "Components/AmmoComp.h"
#include "Components/SphereComponent.h"
#include "Components/StickyGunSkeletalComp.h"
#include "GameFramework/ProjectileMovementComponent.h"

AStickyProjectile::AStickyProjectile()
{
  // Use a sphere as a simple collision representation
  CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
  CollisionComp->InitSphereRadius(5.0f);
  CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
  CollisionComp->OnComponentHit.AddDynamic(
	this, &AStickyProjectile::OnHit);	 // set up a notification for when this component hits something blocking

  // Players can't walk on it
  CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
  CollisionComp->CanCharacterStepUpOn = ECB_No;

  // Set as root component
  RootComponent = CollisionComp;

  // Use a ProjectileMovementComponent to govern this projectile's movement
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
  ProjectileMovement->UpdatedComponent = CollisionComp;
  ProjectileMovement->InitialSpeed = 3000.f;
  ProjectileMovement->MaxSpeed = 3000.f;
  ProjectileMovement->bRotationFollowsVelocity = true;
  ProjectileMovement->bShouldBounce = true;

  // Die after 8 seconds by default
  InitialLifeSpan = 8.0f;
}

void AStickyProjectile::OnHit(
  UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics()) {
	  OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	  auto BaseCharacter = StaticCast<ABaseShooter*>(OtherActor);
		if (BaseCharacter != nullptr) {
		  // Stick to other character skeleton here, at impact point.
		}
	}
}

bool AStickyProjectile::DidPickUp(AActor* OtherActor)
{
	if (OtherActor != nullptr) {
	  auto BaseCharacter = StaticCast<ABaseShooter*>(OtherActor);
	  // TODO: MAKE ABaseShooter::GetStickyGunPtr , StickyGun::GetAmmoCompPtr
	  UAmmoComp* AmmoCompPtr = BaseCharacter->GetStickyGunPtr()->GetAmmoCompPtr();

	  // Destroy projectile if ammo can be picked up
		if (!AmmoCompPtr->IsFullClip()) {
		  Destroy();
		  AmmoCompPtr->ChangeAmmoCount(1);
		  return true;
		}
	}
  return false;
}
