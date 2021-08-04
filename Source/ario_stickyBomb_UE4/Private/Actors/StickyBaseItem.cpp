// Ario Amin - 2021/08

#include "Actors/StickyBaseItem.h"

// Actors
#include "Characters/BaseShooter.h"

// Components
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "Components/StickyGunSkeletalComp.h"

// Interfaces
#include "Interfaces/InteractionUOI.h"

// Helpers
#include "Helpers/CollisionChannels.h"
#include "Helpers/Macros.h"

// Engine Framework
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

// Engine Components
#include <Components/SphereComponent.h>

AStickyBaseItem::AStickyBaseItem()
{
	// Die after 'MaxCurrentLifetime' seconds by default
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);

	SetActorTickEnabled(false);
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyBaseItem::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStickyBaseItem::BeginPlay() { Super::BeginPlay(); }

/** =============================== **/
/** Public Methods: Getters/Setters **/

USphereComponent*					AStickyBaseItem::GetCollisionComp() const { return CollisionComp; }
TArray<UActorComponent*>& AStickyBaseItem::GetReplicatedComponents() { return ReplicatedComponents; };

/** ============================ **/
/** Public Methods: Conditionals **/

/** ================================= **/
/** Protected Methods: Actions/Events **/

/** ======================================= **/
/** Private Methods: Component Initializers **/

void AStickyBaseItem::ConstructCollisionComponent(FName ComponentString, FName CollisionProfileString)
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(ComponentString);
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName(CollisionProfileString);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// SetChannel
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_StickyGun, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Overlap);

	// Set as root component
	RootComponent = CollisionComp;
}
