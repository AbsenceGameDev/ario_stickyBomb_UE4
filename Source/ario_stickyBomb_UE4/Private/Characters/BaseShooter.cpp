// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseShooter.h"

#include "Actors/StickyProjectile.h"
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "Components/StickyGunSkeletalComp.h"
#include "Helpers/CollisionChannels.h"
#include "StickyPlayerState.h"

#include <Animation/AnimInstance.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/InputSettings.h>
#include <HeadMountedDisplayFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <MotionControllerComponent.h>
#include <Net/UnrealNetwork.h>
#include <UObject/ConstructorHelpers.h>
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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_StickyProjectile, ECollisionResponse::ECR_Ignore);
	MeshPtr->SetCollisionObjectType(ECC_CharacterMesh);
	MeshPtr->SetCollisionResponseToChannel(ECC_StickyProjectile, ECollisionResponse::ECR_Overlap);
	// set our turn rates for input
	BaseTurnRate	 = 45.f;
	BaseLookUpRate = 45.f;
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void ABaseShooter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	StickyGun->AttachToComponent(MeshPtr, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	MeshPtr->SetHiddenInGame(false, true);
}

void ABaseShooter::SetupPlayerInputComponent(UInputComponent* InputComponent) {}
void ABaseShooter::BeginInteractItem() { bCanInteract = true; }
void ABaseShooter::EndInteractItem() { bCanInteract = false; }

/** ======================= **/
/** Public Methods: Getters **/

UStickyGunSkeletalComp* ABaseShooter::GetStickyGunPtr() { return StickyGun; }
USkeletalMeshComponent* ABaseShooter::GetMeshPtr() { return MeshPtr; }
UHealthComp*						ABaseShooter::GetHealthCompPtr() { return HealthComponent; }
UAmmoComp*							ABaseShooter::GetAmmoCompPtr() { return AmmoComp; }
UCameraComponent*				ABaseShooter::GetFirstPersonCameraComponent() { return FirstPersonCameraComponent; }

/** ======================= **/
/** Public Methods: Actions **/
void ABaseShooter::SetClosestInteractItem(AStickyProjectile* PickupActor) { ClosestProjectile = PickupActor; }
void ABaseShooter::TryStartFire() { StickyGun->TryStartFire(); }
void ABaseShooter::TryInteractItem()
{
	UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"CHECK IF AUTH FOR PICKUP!\""));
	if (GetLocalRole() < ROLE_Authority) {
		ServerTryInteractItem();
	}
	UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"CHECK IF CAN INTERACT NAD PROJECTILE VALID!\""));
	if (!bCanInteract || ClosestProjectile == NULL) {
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"TRY DIDPICKUP()!\""));
	if (ClosestProjectile->DidPickup(this)) {
		UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"SUCCESS DIDPICKUP()!\""));
		bCanInteract = false;
	}
}

/** ====================== **/
/** Public Methods: UI/HUD **/

void ABaseShooter::TriggerPlayerStateAmmo(int LocalAmmoUpdate)
{
	auto LocalPlayerState = StaticCast<AStickyPlayerState*>(GetPlayerState());

	if (LocalPlayerState != NULL) {
		LocalPlayerState->SetAmmo(LocalAmmoUpdate);
		return;
	}
}

/** ======================= **/
/** Public Methods: VFX/SFX **/

void ABaseShooter::FireGunEffects()
{
	USoundBase*		LocalSoundPtr				= StickyGun->GetFireSound();
	UAnimMontage* LocalAnimMontagePtr = StickyGun->GetFireAnimMontage();

	// try and play the sound if specified
	if (LocalSoundPtr != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, LocalSoundPtr, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (LocalAnimMontagePtr != nullptr) {
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = MeshPtr->GetAnimInstance();
		if (AnimInstance != nullptr) {
			AnimInstance->Montage_Play(LocalAnimMontagePtr, 1.f);
		}
	}
}

/** ================================ **/
/** Protected Methods: Server/Client **/

void ABaseShooter::ServerTryInteractItem_Implementation() { TryInteractItem(); }
bool ABaseShooter::ServerTryInteractItem_Validate() { return true; }

/** ================================== **/
/** Protected Methods: Component Setup **/

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
	// HealthComponent->InitHealthComponent();
}

void ABaseShooter::SetupStickyGun()
{
	StickyGun = CreateDefaultSubobject<UStickyGunSkeletalComp>(TEXT("StickyGun"));
	AmmoComp	= CreateDefaultSubobject<UAmmoComp>(TEXT("AmmoComp"));

	// Initializing using RootComponent for attachment and Offset for placement
	StickyGun->InitStickyGun(this, FVector(100.0f, 0.0f, 10.0f), CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation")));
	TriggerPlayerStateAmmo(AmmoComp->GetAmmo());
}

/** ========================= **/
/** Protected Methods: Inputs **/

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