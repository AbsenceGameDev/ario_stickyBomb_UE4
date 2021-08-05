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
	ConstructStaticMeshComponent();
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

void AStickyPickupContainer::ConstructStaticMeshComponent()
{
	// Create the static mesh component
	MeshComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh"));
	if (Mesh.Succeeded()) {
		MeshComponentPtr->SetStaticMesh(Mesh.Object);
	}

	// TODO: pass the material as a constructor parameter instead of generating it here every spawn
	static ConstructorHelpers::FObjectFinder<UMaterial> BaseMaterial(TEXT("/Game/FirstPerson/Meshes/BaseMaterial.BaseMaterial"));
	if (BaseMaterial.Succeeded()) {
		MeshMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial.Object, MeshComponentPtr, "BaseMaterialInstance");
		MeshMaterialInstance->SetVectorParameterValue("DiffuseColor", FLinearColor::Red);
		MeshComponentPtr->SetMaterial(0, MeshMaterialInstance);

		// MeshMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, MeshComponentPtr);
	}
	MeshComponentPtr->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.12f));
	MeshComponentPtr->SetupAttachment(RootComponent);

	// MeshComponentPtr->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

void AStickyPickupContainer::FillBoxWithPickups()
{
	// Generate enough StickyPickups to fill a given Box, spawn them during BeginPlay
}