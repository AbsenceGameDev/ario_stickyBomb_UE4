// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StickyGunSkeletalComp.h"

#include "Actors/StickyProjectile.h"

UStickyGunSkeletalComp::UStickyGunSkeletalComp()
{
  ProjectileClass = AStickyProjectile::StaticClass();
  AmmoComp = NewObject<UAmmoComp>();

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
  FloatCurve = NewObject<UCurveFloat>();
}

// For THREE_STATE_GATE() to evaluate to true, input 'MustExist' has to be true, the other input is optional
#define THREE_STATE_GATE(MustExist, IsOptional) \
  (MustExist != nullptr && (IsOptional == false)) || (MustExist != nullptr && (IsOptional == true))

void UStickyGunSkeletalComp::OnFire()
{
	// try and fire a projectile
	if ((ProjectileClass != nullptr) && !bDisable && !AmmoComp->IsEmpty()) {
	  UWorld* const World = GetWorld();
		if (World != nullptr) {
		  const FRotator SpawnRotation = OwningCharacter->GetControlRotation();
		  // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to
		  // find the final muzzle position
		  const FVector SpawnLocation =
			((PlacementComp != nullptr) ? PlacementComp->GetComponentLocation() : OwningCharacter->GetActorLocation()) +
			SpawnRotation.RotateVector(GunOffset);

		  // Set Spawn Collision Handling Override
		  FActorSpawnParameters ActorSpawnParams;
		  ActorSpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		  // spawn the projectile at the muzzle and passing Curve into spawned projectile
		  auto LocalProjectileActorPtr =
			World->SpawnActor<AStickyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (LocalProjectileActorPtr != nullptr) {	 // Don't spawn
			  LocalProjectileActorPtr->SetCurve(FloatCurve);
			}
		  AmmoComp->ChangeAmmoCount(-1);
		  OwningCharacter->FireGunEffects(this);
		}
	}
}

void UStickyGunSkeletalComp::InitStickyGun(ABaseShooter* Caller, FVector LocalGunOffset, USceneComponent* MuzzlePlacementComp)
{
	// Set the skeletal mesh asset of the USkeletalMeshComponent
	if (MeshPtr == nullptr) {
	  static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(TEXT("/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun"));
	  MeshPtr = SkeletalMeshObj.Object;
	}
  SetSkeletalMesh(MeshPtr, true);

  SetOnlyOwnerSee(false);	 // otherwise won't be visible in the multiplayer
  bCastDynamicShadow = false;
  CastShadow = false;

	if (Caller != nullptr) {
	  bDisable = false;
	  SetupAttachment(Caller->GetRootComponent());
	  OwningCharacter = Caller;
	}

	if (MuzzlePlacementComp != nullptr) {
	  MuzzlePlacementComp->SetupAttachment(this);
	  MuzzlePlacementComp->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	  PlacementComp = MuzzlePlacementComp;
	}

  GunOffset = LocalGunOffset;
  GenerateCurve();
}

UAmmoComp* UStickyGunSkeletalComp::GetAmmoCompPtr()
{
  return AmmoComp;
}

void UStickyGunSkeletalComp::GenerateCurve()
{
  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"CURVE GENERATED!\""));
  float SignageAlternator = -1.0f;
  float LocalTimeStep = 0.0f;
  float LocalMaxTime = 8.0f;
  float LocalValue = 0.0f;	  // Clamp between [ -1, +1 ]
	for (; LocalTimeStep < LocalMaxTime;) {
	  GeneratedRichCurve->AddKey(LocalTimeStep, LocalValue);
	  LocalTimeStep += 0.05f;
	  SignageAlternator *= -1.0f;
	  LocalValue = SignageAlternator * (LocalTimeStep / LocalMaxTime);
	}

  auto CurveList = FloatCurve->GetCurves();
  CurveList.Add(FRichCurveEditInfo(GeneratedRichCurve, FName{TEXT("GeneratedRichCurve")}));
}
