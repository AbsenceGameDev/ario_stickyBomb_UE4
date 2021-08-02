// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StickyGunSkeletalComp.h"

#include "Actors/StickyProjectile.h"
#include "Components/AmmoComp.h"
#include "Helpers/CollisionChannels.h"

#include <Components/SkeletalMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

UStickyGunSkeletalComp::UStickyGunSkeletalComp()
{
	ProjectileClass = AStickyProjectile::StaticClass();

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundBaseObj(
		TEXT("/Game/FirstPerson/Audio/FirstPersonTemplateWeaponFire02.FirstPersonTemplateWeaponFire02"));
	if (SoundBaseObj.Succeeded()) {
		FireSound = SoundBaseObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMontageObj(
		TEXT("/Game/FirstPerson/Animations/FirstPersonFire_Montage.FirstPersonFire_Montage"));
	if (AnimMontageObj.Succeeded()) {
		FireAnimation = AnimMontageObj.Object;
	}

	// GeneratedRichCurve = new FRichCurve();
	FloatCurve = NewObject<UCurveFloat>();
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

/** ================================ **/
/** Public Methods: Client interface **/

void UStickyGunSkeletalComp::InitStickyGun(ABaseShooter* Caller, FVector LocalGunOffset, USceneComponent* MuzzlePlacementComp)
{
	// Set the skeletal mesh asset of the USkeletalMeshComponent
	if (MeshPtr == nullptr) {
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(TEXT("/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun"));
		MeshPtr = SkeletalMeshObj.Object;
	}
	SetSkeletalMesh(MeshPtr, true);

	SetOnlyOwnerSee(false);		 // otherwise won't be visible in the multiplayer
	bCastDynamicShadow = false;
	CastShadow				 = false;

	if (Caller != nullptr) {
		bDisable = false;
		SetupAttachment(Caller->GetRootComponent());
		OwningCharacter = Caller;
		AmmoComp				= OwningCharacter->GetAmmoComp();
	}

	if (MuzzlePlacementComp != nullptr) {
		MuzzlePlacementComp->SetupAttachment(this);
		MuzzlePlacementComp->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
		PlacementComp = MuzzlePlacementComp;
	}

	GunOffset = LocalGunOffset;
	this->SetCollisionObjectType(ECC_StickyGun);
	this->SetCollisionResponseToChannel(ECC_StickyProjectile, ECollisionResponse::ECR_Ignore);

	GenerateCurve();

	OwningCharacter->TriggerPlayerStateAmmo(3);
}

void UStickyGunSkeletalComp::TryStartFire() { OnFire(); }

/** ================================= **/
/** Public Methods: Networked VFX/SFX **/

void UStickyGunSkeletalComp::MulticastFireGunEffects_Implementation(AStickyProjectile* LocalProjectileActorPtr)
{
	SuccessFireEffects();
	// try and play the sound if specified
	// if (LocalProjectileActorPtr == NULL) {
	// 	FailFireEffects();
	// 	return;
	// }
	// SuccessFireEffects();
}

/** ========================== **/
/** Protected Methods: VFX/SFX **/
void UStickyGunSkeletalComp::SuccessFireEffects()
{
	if (FireSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetComponentLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr) {
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetAnimInstance();
		if (AnimInstance != nullptr) {
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}
void UStickyGunSkeletalComp::FailFireEffects()
{
	UE_LOG(LogTemp, Warning, TEXT("FAILED FIRE FX!"));
	// if (FireSound != nullptr) {
	//   // FireSound->
	// 	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetComponentLocation());
	// }
	//
	// // try and play a firing animation if specified
	// if (FireAnimation != nullptr) {
	// 	// Get the animation object for the arms mesh
	// 	UAnimInstance* AnimInstance = GetAnimInstance();
	// 	if (AnimInstance != nullptr) {
	// 		AnimInstance->Montage_Play(FireAnimation, 1.f);
	// 	}
	// }
}

/** =============================== **/
/** Public Methods: Getters/Setters **/
UAmmoComp*		UStickyGunSkeletalComp::GetAmmoComp() { return AmmoComp; }
ABaseShooter* UStickyGunSkeletalComp::GetOwningCharacter() { return OwningCharacter; }
USoundBase*		UStickyGunSkeletalComp::GetFireSound() { return FireSound; }
UAnimMontage* UStickyGunSkeletalComp::GetFireAnimMontage() { return FireAnimation; }

/** ======================================== **/
/** Protected Methods: Client/Server actions **/

void UStickyGunSkeletalComp::OnFire()
{
	// Request Server if we by some reason still are Client
	if (GetOwnerRole() < ROLE_Authority) {
		ServerOnFire();
		return;
	}

	if ((ProjectileClass == nullptr) || bDisable || AmmoComp->IsEmpty()) {
		return;
	}

	// try and fire a projectile
	UWorld* const World = GetWorld();
	if (World == nullptr || AmmoComp->IsEmpty()) {
		return;
	}

	const FRotator SpawnRotation = OwningCharacter->GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to
	// find the final muzzle position
	const FVector SpawnLocation =
		((PlacementComp != nullptr) ? PlacementComp->GetComponentLocation() : OwningCharacter->GetActorLocation()) +
		SpawnRotation.RotateVector(GunOffset);

	// Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	auto SpawnTransform															= FTransform(SpawnRotation, SpawnLocation, FVector(1.0f, 1.0f, 1.0f));

	// spawn the projectile at the muzzle and passing Curve into spawned projectile
	auto LocalProjectileActorPtr = World->SpawnActorDeferred<AStickyProjectile>(
		ProjectileClass, SpawnTransform, OwningCharacter, OwningCharacter, ActorSpawnParams.SpawnCollisionHandlingOverride);
	if (LocalProjectileActorPtr == nullptr) {
		return;
	}

	PrepDeferredSpawnProjectile(LocalProjectileActorPtr);
	if (FinishSpawnProjectile(LocalProjectileActorPtr, SpawnTransform)) {
		AmmoComp->TryFire();
		OwningCharacter->TriggerPlayerStateAmmo(AmmoComp->GetAmmo());
	}
}

void UStickyGunSkeletalComp::PrepDeferredSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr)
{
	LocalProjectileActorPtr->SetCurve(FloatCurve);
	LocalProjectileActorPtr->SetMaxPossibleLifetime(100.0f);

	UTimelineComponent* StickyTimelineComp = LocalProjectileActorPtr->GetTimelineComp();

	if (LocalProjectileActorPtr->GetCurve() != nullptr && StickyTimelineComp != nullptr) {
		// Indicate it comes from source code, and is native to the actor
		StickyTimelineComp->CreationMethod = EComponentCreationMethod::Native;

		LocalProjectileActorPtr->GetReplicatedComponents().Add(StickyTimelineComp);		 // Add to array so it gets saved
		StickyTimelineComp->SetNetAddressable();		// This component has a stable name that can be referenced for replication

		// Set which object the timeline should drive properties on
		StickyTimelineComp->SetPropertySetObject(LocalProjectileActorPtr);

		StickyTimelineComp->SetDirectionPropertyName(FName("TimelineDirection"));
		StickyTimelineComp->SetLooping(false);
		StickyTimelineComp->SetTimelineLength(LocalProjectileActorPtr->GetMaxLifetime());
		StickyTimelineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		StickyTimelineComp->SetPlaybackPosition(0.0f, false);

		// Add the float curve to the timeline and connect it to your timelines's interpolation function
		FOnTimelineFloat			 OnTimelineTickDelegate;
		FOnTimelineEventStatic OnTimelineFinishedCallback;
		OnTimelineTickDelegate.BindUFunction(LocalProjectileActorPtr, FName{TEXT("ModulateColor")});
		OnTimelineFinishedCallback.BindUFunction(LocalProjectileActorPtr, FName{TEXT("TriggerExplosionFX")});
		StickyTimelineComp->AddInterpFloat(LocalProjectileActorPtr->GetCurve(), OnTimelineTickDelegate);
		StickyTimelineComp->SetTimelineFinishedFunc(OnTimelineFinishedCallback);

		// StickyTimelineComp->RegisterComponent();
		// PlayTimeline();
		return;
	}
}

bool UStickyGunSkeletalComp::FinishSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr, FTransform const& SpawnTransform)
{
	LocalProjectileActorPtr =
		StaticCast<AStickyProjectile*>(UGameplayStatics::FinishSpawningActor(LocalProjectileActorPtr, SpawnTransform));
	if (IsValid(LocalProjectileActorPtr)) {
		MulticastFireGunEffects(LocalProjectileActorPtr);
		return true;
	}
	return false;
}

void UStickyGunSkeletalComp::ServerOnFire_Implementation() { OnFire(); }

// intended for anti-cheat, validates code
bool UStickyGunSkeletalComp::ServerOnFire_Validate() { return true; }

/** ================================= **/
/** Protected Methods: Client actions **/

void UStickyGunSkeletalComp::GenerateCurve()
{
	float SignageAlternator = -1.0f;
	float LocalTimeStep			= 0.0f;
	float LocalMaxTime			= 8.0f;
	float LocalValue				= 0.0f;		 // Clamp between [ -1, +1 ]
	for (; LocalTimeStep < LocalMaxTime;) {
		GeneratedRichCurve.AddKey(LocalTimeStep, LocalValue);
		LocalTimeStep += 0.05f;
		SignageAlternator *= -1.0f;
		LocalValue = SignageAlternator * (LocalTimeStep / LocalMaxTime);
	}

	auto CurveList = FloatCurve->GetCurves();
	CurveList.Add(FRichCurveEditInfo(&GeneratedRichCurve, FName{TEXT("GeneratedRichCurve")}));
}