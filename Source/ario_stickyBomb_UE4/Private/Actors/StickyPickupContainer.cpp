/**
 * @author  Ario Amin
 * @file    Actors/StickyPickupContainer.cpp
 * @class   AStickyPickupContainer
 * @brief   A simple actor which holds a static mesh container
 */

#include "Actors/StickyPickupContainer.h"

// Helpers
#include "Helpers/CollisionChannels.h"
#include "Helpers/Macros.h"

// Engine Frameworks
#include <Kismet/KismetSystemLibrary.h>
#include <Net/UnrealNetwork.h>

// Engine Helpers
#include <UObject/ConstructorHelpers.h>

// Engine Classes/Types
#include <Materials/MaterialInstanceDynamic.h>

AStickyPickupContainer::AStickyPickupContainer()
{
	ConstructStaticMeshComponents();
	SetCollisionResponses();

	PrimaryActorTick.bCanEverTick					 = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);
}

/* ============================ */
/** Inherited Methods: Overrides */

void AStickyPickupContainer::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AStickyPickupContainer::BeginPlay() { Super::BeginPlay(); }
void AStickyPickupContainer::EndInteractItem() {}
void AStickyPickupContainer::TryInteractItem() {}

/* =============================== */
/* Public Methods: Getters/Setters */

/* ======================================= */
/* Private Methods: Component Initializers */

void AStickyPickupContainer::SetCollisionResponses()
{
	// Mesh?  ->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// Mesh?  ->SetCollisionResponseToChannel(ECC_StickyGun, ECollisionResponse::ECR_Ignore);
	// Mesh?  ->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	// Mesh?  ->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);
}

void AStickyPickupContainer::ConstructStaticMeshComponents() {}
