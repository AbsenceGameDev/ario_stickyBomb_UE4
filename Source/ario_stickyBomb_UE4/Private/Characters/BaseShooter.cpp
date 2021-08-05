/**
 * @author  Ario Amin
 * @file    Characters/ABaseShooter.cpp
 * @class   ABomberman
 * @brief   Inherits from ACharacter & IInteractionUOI
 * @details Derived from ACharacter and implements a StickyGunSkeletalComponent, HealthComponent and a AmmoComponent
 * @todo    Currently this class is fairly bloated.
 *          1. Move some of the functions of ABaseShooter into Bomberman, or rather
 *          rewrite some as virtual and implement them in child classes such as ABomberMan.
 *          2. Move methods such as those regarding to input setup to the player controller class
 */
#include "Characters/BaseShooter.h"

// General
#include "Actors/StickyProjectile.h"
#include "Helpers/CollisionChannels.h"
#include "StickyPlayerState.h"

// Components
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "Components/StickyGunSkeletalComp.h"
#include "Components/StickyLinetraceComp.h"

// Engine Components
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <MotionControllerComponent.h>

// Engine Frameworks
#include <Animation/AnimInstance.h>
#include <GameFramework/DamageType.h>
#include <GameFramework/InputSettings.h>
#include <Net/UnrealNetwork.h>

// Engine Helpers
#include <Animation/AnimBlueprint.h>
#include <Kismet/GameplayStatics.h>
#include <UObject/ConstructorHelpers.h>

// Probably not needed
#include <HeadMountedDisplayFunctionLibrary.h>
#include <XRMotionControllerBase.h>		 // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);
// Sets default values
ABaseShooter::ABaseShooter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	InitCamera();
	InitSkeletalBody();
	InitActorComponents();
	SetupStickyGun();

	SetupCollision();
	// set our turn rates for input
	BaseTurnRate	 = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = true;

	// this->OnTakeAnyDamage.AddDynamic(this, &ABaseShooter::ServerTakeDamage);
	// this->OnTakeRadialDamage.AddDynamic(this, &ABaseShooter::ServerTakeDamage);
}

// no known conversion from
// 'void (ABaseShooter::*)(AActor *, float, const UDamageType *, AController *, AActor *)'
// to
// 'void (ABaseShooter::*)(AActor *, float, const UDamageType *, FVector, FHitResult, AController *, AActor *)' for 2nd argument

/* ============================ */
/** Inherited Methods: Overrides */
void ABaseShooter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	StickyGun->AttachToComponent(MeshPtr, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	MeshPtr->SetHiddenInGame(false, true);
}

void ABaseShooter::SetupPlayerInputComponent(UInputComponent* InputComponent) {}
void ABaseShooter::TryInteractItem()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerTryInteractItem();
		return;
	}
	LinetraceComp->SetComponentTickEnabled(true);
}
void ABaseShooter::EndInteractItem()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerEndInteractItem();
		return;
	}
	LinetraceComp->SetComponentTickEnabled(false);
}

float ABaseShooter::TakeDamage(
	float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Damage Received %f"), DamageAmount);
#endif		// STICKY_DEBUG
	auto DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
	ServerTakeDamage(this, DamageAmount, DamageType, EventInstigator, DamageCauser);
	return 0.0f;
}

void ABaseShooter::ServerTakeDamage_Implementation(
	AActor* ThisActor, float DamageAmount, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser)
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"PLAYER HIT!\""));
#endif		// STICKY_DEBUG
	HealthComponent->TryTakeDamage(ThisActor, DamageAmount, DamageType, EventInstigator, DamageCauser);
}

/* ======================= */
/* Public Methods: Getters */

UStickyGunSkeletalComp* ABaseShooter::GetStickyGun() { return StickyGun; }
USkeletalMeshComponent* ABaseShooter::GetCharMesh() { return MeshPtr; }
UHealthComp*						ABaseShooter::GetHealthComp() { return HealthComponent; }
UAmmoComp*							ABaseShooter::GetAmmoComp() { return AmmoComp; }
UCameraComponent*				ABaseShooter::GetFirstPersonCameraComponent() { return FirstPersonCameraComponent; }

/* ======================= */
/* Public Methods: Actions */
void ABaseShooter::TryStartFire() { StickyGun->TryStartFire(); }

/* ====================== */
/* Public Methods: UI/HUD */

void ABaseShooter::TriggerPlayerStateAmmo(int LocalAmmoUpdate)
{
	auto LocalPlayerState = StaticCast<AStickyPlayerState*>(GetPlayerState());

	if (LocalPlayerState != NULL) {
		LocalPlayerState->SetAmmo(LocalAmmoUpdate);
		return;
	}
}

/* ================================ */
/* Protected Methods: Server/Client */
void ABaseShooter::ServerTryInteractItem_Implementation() { TryInteractItem(); }
bool ABaseShooter::ServerTryInteractItem_Validate() { return true; }

void ABaseShooter::ServerEndInteractItem_Implementation() { EndInteractItem(); }
bool ABaseShooter::ServerEndInteractItem_Validate() { return true; }

void ABaseShooter::ServerUndoRagdoll_Implementation() { MulticastUndoRagdoll(); }
bool ABaseShooter::ServerUndoRagdoll_Validate() { return !HealthComponent->IsDead(); }

void ABaseShooter::ServerTriggerRagdoll_Implementation()
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("SERVER TRIGGER RAGDOLL"));
#endif		// STICKY_DEBUG
	MulticastTriggerRagdoll();
}
bool ABaseShooter::ServerTriggerRagdoll_Validate() { return HealthComponent->IsDead(); }

/* ================================== */
/* Protected Methods: Component Setup */

void ABaseShooter::InitSkeletalBody()
{
	MeshPtr = GetMesh();

	// Set the skeletal mesh asset of the USkeletalMeshComponent
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(
		TEXT("/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms"));
	USkeletalMesh* ArmsSkeletalMesh = SkeletalMeshObj.Object;
	MeshPtr->SetSkeletalMesh(ArmsSkeletalMesh, true);

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> FpsAnimClassObj(TEXT("/Game/FirstPerson/Animations/FirstPerson_AnimBP"));
	MeshPtr->SetAnimInstanceClass(FpsAnimClassObj.Object->GeneratedClass);

	MeshPtr->SetOnlyOwnerSee(false);
	MeshPtr->SetupAttachment(FirstPersonCameraComponent);
	MeshPtr->bCastDynamicShadow = false;
	MeshPtr->CastShadow					= false;
	MeshPtr->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	MeshPtr->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

void ABaseShooter::InitCamera()
{
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));		 // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

void ABaseShooter::InitActorComponents()
{
	HealthComponent = CreateDefaultSubobject<UHealthComp>(TEXT("HealthSystem"));
	LinetraceComp		= CreateDefaultSubobject<UStickyLinetraceComp>(TEXT("LinetraceInteractComponent"));

	LinetraceComp->SetupAttachment(FirstPersonCameraComponent);
	LinetraceComp->SetComponentTickEnabled(false);
	// LinetraceComp->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));	// Position the same as camera
}

void ABaseShooter::SetupStickyGun()
{
	StickyGun = CreateDefaultSubobject<UStickyGunSkeletalComp>(TEXT("StickyGun"));
	AmmoComp	= CreateDefaultSubobject<UAmmoComp>(TEXT("AmmoComp"));

	// Initializing using RootComponent for attachment and Offset for placement
	StickyGun->InitStickyGun(this, FVector(100.0f, 0.0f, 10.0f), CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation")));
	TriggerPlayerStateAmmo(AmmoComp->GetAmmo());
}

void ABaseShooter::SetupCollision()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_StickyProjectile, ECollisionResponse::ECR_Ignore);
	MeshPtr->SetCollisionObjectType(ECC_CharacterMesh);
	MeshPtr->SetCollisionResponseToChannel(ECC_StickyProjectile, ECollisionResponse::ECR_Block);
}

void ABaseShooter::MulticastUndoRagdoll_Implementation()
{
	SetReplicateMovement(true);
	/* Re-connect to server, maybe with CallPreReplication */

	/* Disable all collision on capsule */
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	if (bIsRagdoll) {
		// Ragdoll
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = false;

		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp) {
			CharacterComp->SetMovementMode(EMovementMode::MOVE_Walking, 0);
			CharacterComp->SetComponentTickEnabled(true);
		}

		// SetLifeSpan(10.0f);
		bIsRagdoll = false;
	}
}

void ABaseShooter::MulticastTriggerRagdoll_Implementation()
{
#ifdef STICKY_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("MULTICAST TRIGGER RAGDOLL"));
#endif		// STICKY_DEBUG
	SetReplicateMovement(false);
	TearOff();

	DetachFromControllerPendingDestroy();

	/* Reenable collision on capsule and mesh */
	SetupCollision();

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	if (!bIsRagdoll) {
		// Ragdoll
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp) {
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}

		SetLifeSpan(10.0f);
		bIsRagdoll = true;
	}

	/* Apply physics impulse on the bone of the player mesh we hit */
	GetMesh()->AddRadialImpulse(GetActorLocation(), 300.0f, 10000, ERadialImpulseFalloff::RIF_Linear);
}

/* ========================= */
/* Protected Methods: Inputs */
void ABaseShooter::MoveForward(float Value)
{
	if (Value != 0.0f) {
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABaseShooter::MoveRight(float Value)
{
	if (Value != 0.0f) {
		AddMovementInput(GetActorRightVector(), Value);
	}
}

// calculate delta for this frame from the rate information
void ABaseShooter::TurnAtRate(float Rate) { AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); }
void ABaseShooter::LookUpAtRate(float Rate) { AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); }

void ABaseShooter::CreateNewAxisMapping(FName DesiredAxisName, FKey DesiredAxisKey)
{
	const auto AxisMapping = FInputAxisKeyMapping(DesiredAxisName, DesiredAxisKey, 1);

	const UInputSettings* InputSettings = GetDefault<UInputSettings>();
	((UInputSettings*) InputSettings)->AddAxisMapping(AxisMapping);
	((UInputSettings*) InputSettings)->SaveKeyMappings();
}

void ABaseShooter::CreateNewActionMapping(FName DesiredActionName, FKey DesiredActionKey)
{
	const auto ActionMapping = FInputActionKeyMapping(DesiredActionName, DesiredActionKey, false, false, false, false);

	const UInputSettings* InputSettings = GetDefault<UInputSettings>();
	((UInputSettings*) InputSettings)->AddActionMapping(ActionMapping);
	((UInputSettings*) InputSettings)->SaveKeyMappings();
}
