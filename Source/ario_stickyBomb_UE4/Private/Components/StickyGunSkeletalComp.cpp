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

	GeneratedRichCurve = new FRichCurve();
	FloatCurve				 = NewObject<UCurveFloat>();
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
		AmmoComp				= OwningCharacter->GetAmmoCompPtr();
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
}

void UStickyGunSkeletalComp::TryStartFire() { OnFire(); }

/** =============================== **/
/** Public Methods: Getters/Setters **/
UAmmoComp*		UStickyGunSkeletalComp::GetAmmoCompPtr() { return AmmoComp; }
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
	if (World == nullptr) {
		return;
	}

	int LocalAmmoPrev = AmmoComp->GetAmmo();
	AmmoComp->TryFire();
	int LocalAmmoDiff = LocalAmmoPrev - AmmoComp->GetAmmo();
	if (LocalAmmoDiff == 0) {
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
		ProjectileClass, SpawnTransform, OwningCharacter, OwningCharacter,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

	PrepDeferredSpawnProjectile(LocalProjectileActorPtr);
	FinishSpawnProjectile(LocalProjectileActorPtr, SpawnTransform);
}

void UStickyGunSkeletalComp::PrepDeferredSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr)
{
	if (LocalProjectileActorPtr == nullptr) {
		return;
	}
	LocalProjectileActorPtr->SetCurve(FloatCurve);
	LocalProjectileActorPtr->SetMaxPossibleLifetime(100.0f);

	UTimelineComponent* StickyTimelineComp = LocalProjectileActorPtr->GetTimelineComp();

	if (LocalProjectileActorPtr->GetCurve() != nullptr && StickyTimelineComp != nullptr) {
		UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"GENERATED CURVE PASSED SUCCESSFULLY \""));

		StickyTimelineComp->CreationMethod =
			EComponentCreationMethod::Native;		 // Indicate it comes from source code, and is native to the actor
		LocalProjectileActorPtr->GetReplicatedComponents().Add(StickyTimelineComp);		 // Add to array so it gets saved
		StickyTimelineComp->SetNetAddressable();		// This component has a stable name that can be referenced for replication

		StickyTimelineComp->SetPropertySetObject(
			LocalProjectileActorPtr);		 // Set which object the timeline should drive properties on
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
	UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"GENERATED CURVE DID NOT PASS SUCCESSFULLY \""));
}

void UStickyGunSkeletalComp::FinishSpawnProjectile(AStickyProjectile* LocalProjectileActorPtr, FTransform const& SpawnTransform)
{
	if (LocalProjectileActorPtr == nullptr) {
		return;
	}
	LocalProjectileActorPtr =
		StaticCast<AStickyProjectile*>(UGameplayStatics::FinishSpawningActor(LocalProjectileActorPtr, SpawnTransform));
	OwningCharacter->FireGunEffects(this);
}

void UStickyGunSkeletalComp::ServerOnFire_Implementation() { OnFire(); }

// intended for anti-cheat, validates code
bool UStickyGunSkeletalComp::ServerOnFire_Validate() { return true; }

/** ================================= **/
/** Protected Methods: Client actions **/

void UStickyGunSkeletalComp::GenerateCurve()
{
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"CURVE GENERATED!\""));
	float SignageAlternator = -1.0f;
	float LocalTimeStep			= 0.0f;
	float LocalMaxTime			= 8.0f;
	float LocalValue				= 0.0f;		 // Clamp between [ -1, +1 ]
	for (; LocalTimeStep < LocalMaxTime;) {
		GeneratedRichCurve->AddKey(LocalTimeStep, LocalValue);
		LocalTimeStep += 0.05f;
		SignageAlternator *= -1.0f;
		LocalValue = SignageAlternator * (LocalTimeStep / LocalMaxTime);
	}

	auto CurveList = FloatCurve->GetCurves();
	CurveList.Add(FRichCurveEditInfo(GeneratedRichCurve, FName{TEXT("GeneratedRichCurve")}));

	if (GeneratedRichCurve == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"RICH CURVE NOT PROPERLY GENERATED!\""));
	}
	if (FloatCurve == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"FLOAT CURVE NOT PROPERLY COPIED!\""));
	}
}