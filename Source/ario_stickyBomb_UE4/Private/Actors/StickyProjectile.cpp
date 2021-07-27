// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/StickyProjectile.h"

#include "Characters/BaseShooter.h"
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
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
  // Maybe move to defaultingthese values by deriving a class from
  // UProjectileMovementComponent and set their default values to the ones below
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
  ProjectileMovement->UpdatedComponent = CollisionComp;
  ProjectileMovement->InitialSpeed = 3000.f;
  ProjectileMovement->MaxSpeed = 3000.f;
  ProjectileMovement->bRotationFollowsVelocity = true;
  ProjectileMovement->bShouldBounce = true;

  SetActorHiddenInGame(false);

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

  StickyTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("StickyTimelineComp"));

  // Die after 8 seconds by default
  InitialLifeSpan = MaxCurrentLifetime;
  PrimaryActorTick.bCanEverTick = true;
  SetActorTickEnabled(true);
}

void AStickyProjectile::BeginPlay()
{
  Super::BeginPlay();

	if (StickyTimelineCurve != nullptr) {
	  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"GENERATED CURVE PASSED SUCCESSFULLY \""));
	  FOnTimelineFloat OnTimelineTickDelegate;
	  FOnTimelineEventStatic OnTimelineFinishedCallback;

	  StickyTimelineComp->CreationMethod =
		EComponentCreationMethod::Native;					 // Indicate it comes from source code, and is native to the actor
	  this->ReplicatedComponents.Add(StickyTimelineComp);	 // Add to array so it gets saved
	  StickyTimelineComp->SetNetAddressable();	  // This component has a stable name that can be referenced for replication

	  StickyTimelineComp->SetPropertySetObject(this);	 // Set which object the timeline should drive properties on
	  StickyTimelineComp->SetDirectionPropertyName(FName("TimelineDirection"));

	  StickyTimelineComp->SetLooping(false);
	  StickyTimelineComp->SetTimelineLength(MaxCurrentLifetime);
	  StickyTimelineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	  StickyTimelineComp->SetPlaybackPosition(0.0f, false);

	  // Add the float curve to the timeline and connect it to your timelines's interpolation function
	  OnTimelineTickDelegate.BindUFunction(this, FName{TEXT("ModulateColor")});
	  OnTimelineFinishedCallback.BindUFunction(this, FName{TEXT("TriggerExplosion")});
	  StickyTimelineComp->AddInterpFloat(StickyTimelineCurve, OnTimelineTickDelegate);
	  StickyTimelineComp->SetTimelineFinishedFunc(OnTimelineFinishedCallback);

	  // StickyTimelineComp->RegisterComponent();
	  PlayTimeline();
	  return;
	}
  UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"GENERATED CURVE DID NOT PASS SUCCESSFULLY \""));
}

void AStickyProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

	if (StickyTimelineComp != nullptr) {
	  StickyTimelineComp->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}

void AStickyProjectile::ModulateColor(const float InterpValue)
{
  static int TestModulator = 1;	   // Only sets once per object
  TestModulator *= -1;			   // Flip value

  float TimelinePosition = StickyTimelineComp->GetPlaybackPosition() / MaxPossibleLifetime;
  auto Fmodifier = StickyTimelineCurve->GetFloatValue(TimelinePosition);
  auto ValMod = TestModulator + Fmodifier;
  auto ColorAdditive = (FLinearColor(ValMod, ValMod, ValMod, 0.0f));
  BaseColor += ColorAdditive;
  // MeshMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);

  //
  // MeshComponentPtr->GetMaterial(0)
  // ->GetMaterialResource(ERHIFeatureLevel::Type::Num, EMaterialQualityLevel::Type::Num)

  auto DynamicMaterialInstance = MeshComponentPtr->CreateDynamicMaterialInstance(0);
  DynamicMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);
  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"TimelinePosition: %f -- Fmodifier: %f -- ValMod: %f \""), TimelinePosition,
	Fmodifier, ValMod);
  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"COLOR.R: %f -- COLOR.G: %f -- COLOR.B: %f \""), ColorAdditive.R, ColorAdditive.G,
	ColorAdditive.B);
}

void AStickyProjectile::TriggerExplosion()
{
  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"COLOR MODULATION: SUCCESS??\""));
  // Kill or Damage actor, trigger HealthComponent OnDamage
  // at the very least on any BaseShooter derived character within DamageRadius
}

void AStickyProjectile::OnHit(
  UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and attach projectile if we hit a player/character that is not the object causing the hit
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
	  OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	  auto* CastOtherBaseShooter = StaticCast<ABaseShooter*>(OtherActor);

	  auto* CastOwnerBaseShooter = StaticCast<ABaseShooter*>(this->GetOwner());

		if (this->GetOwner() == nullptr) {
		  UE_LOG(LogTemp, Warning, TEXT("Projectile Has No Owner!"));
		  return;
		}
	  UE_LOG(LogTemp, Log, TEXT("Projectile Owner Name: %s"), *this->GetOwner()->GetName());

		if (CastOwnerBaseShooter == nullptr || CastOtherBaseShooter == nullptr) {
		  return;
		}
		if (CastOwnerBaseShooter->GetUniqueID() != OtherActor->GetUniqueID()) {
		  bool bLocalIsDead = CastOtherBaseShooter->GetHealthCompPtr()->IsDead();
		  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"HIT YOU!\""));
		  // Stick to other character skeleton here, at impact point.
		  // AttachToComponent(CastOtherBaseShooter->GetMeshPtr(), FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);

		  // Seems to crash the timeline component, not using a timelinecomp seems less errorprone
		  // StickyTimelineComp->SetPlayRate(2.0f);
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

void AStickyProjectile::SetCurve(UCurveFloat* InCurve)
{
  StickyTimelineCurve = InCurve;

	if (StickyTimelineCurve != nullptr) {
	  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"INCURVE PROPERLY PASSED TO FUNCTION!\""));
	}
	if (StickyTimelineCurve != nullptr) {
	  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"STICKYTIMELINECURVE PROPERLY COPIED!\""));
	}
}

void AStickyProjectile::PlayTimeline()
{
	if (StickyTimelineComp != nullptr) {
	  StickyTimelineComp->PlayFromStart();
	}
}

float AStickyProjectile::GetDamageRadius() const
{
  return DamageRadius;
}
float AStickyProjectile::GetDamageAmount() const
{
  return DamageValue;
}

void AStickyProjectile::SetDamageRadius(float InRadius)
{
  // Apply effect on InRadius before setting DamageRadius?
  DamageRadius = InRadius;
}

void AStickyProjectile::SetDamageAmount(float InDamage)
{
  DamageValue = InDamage;
}