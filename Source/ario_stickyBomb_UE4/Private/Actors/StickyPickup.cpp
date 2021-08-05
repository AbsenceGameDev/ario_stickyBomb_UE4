/**
 * @author  Ario Amin
 * @file    Actors/StickyPickup.h
 * @class   AStickyPickup
 * @brief   A simple pickup actor
 * @details Has rudimentary pickup logic which sends it to the players ammo component
 **/
#include "Actors/StickyPickup.h"

// General
#include "Characters/BaseShooter.h"
#include "Interfaces/InteractionUOI.h"

// Component
#include "Components/AmmoComp.h"
#include "Components/StickyGunSkeletalComp.h"

// Helpers
#include "Helpers/CollisionChannels.h"
#include "Helpers/Macros.h"

// Engine Frameworks
#include <Kismet/KismetSystemLibrary.h>
#include <Net/UnrealNetwork.h>

// Engine Helpers
#include <UObject/ConstructorHelpers.h>

// Engine Components
#include <Components/SphereComponent.h>

// Engine Classes/Types
#include <Materials/MaterialInstanceDynamic.h>

AStickyPickup::AStickyPickup()
{
	ConstructCollisionComponent();
	ConstructStaticMeshComponent();

	PrimaryActorTick.bCanEverTick					 = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval					 = 1.0f;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyPickup::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AStickyPickup::BeginPlay() { Super::BeginPlay(); }
void AStickyPickup::EndInteractItem() {}
void AStickyPickup::TryInteractItem()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(false);
	Destroy();
}

/** =============================== **/
/** Public Methods: Getters/Setters **/
USphereComponent* AStickyPickup::GetCollisionComp() const { return CollisionComp; }

/** ============================ **/
/** Public Methods: Conditionals **/
bool AStickyPickup::DidPickup(AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority) {
		auto BaseCharacter = StaticCast<ABaseShooter*>(OtherActor);
		if (BaseCharacter != NULL) {
			UAmmoComp* AmmoCompPtr = BaseCharacter->GetStickyGun()->GetAmmoComp();

			// Destroy projectile if ammo can be picked up
			if (!AmmoCompPtr->IsFullClip()) {
				AmmoCompPtr->TryPickupRound();

				// Maybe should return a bool on TryPickUpRound?
				BaseCharacter->TriggerPlayerStateAmmo(AmmoCompPtr->GetAmmo());
				Destroy();
				return true;
			}
		}
	}
	return false;
}

/** ======================================= **/
/** Private Methods: Component Initializers **/

void AStickyPickup::ConstructCollisionComponent()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	CollisionComp->SetCollisionObjectType(ECC_StickyProjectile);
	SetCollisionResponses();

	// Set as root component
	RootComponent = CollisionComp;
}

void AStickyPickup::SetCollisionResponses()
{
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_StickyGun, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);
}

// AStickyPickup::OnBounce()

void AStickyPickup::ConstructStaticMeshComponent()
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
	}
	MeshComponentPtr->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.12f));
	MeshComponentPtr->SetupAttachment(RootComponent);
}
