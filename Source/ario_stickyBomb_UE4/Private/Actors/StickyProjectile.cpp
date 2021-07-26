// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/StickyProjectile.h"

#include "Characters/BaseShooter.h"
#include "Components/AmmoComp.h"
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
	  MeshMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial.Object, MeshComponentPtr, "BaseMaterial");
	  MeshMaterialInstance->SetVectorParameterValue("DiffuseColor", FLinearColor::Red);
	  MeshComponentPtr->SetMaterial(0, MeshMaterialInstance);

	  // MeshMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, MeshComponentPtr);
	}
  MeshComponentPtr->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.12f));
  MeshComponentPtr->SetupAttachment(RootComponent);

  // Init timeline and curve

  // Die after 8 seconds by default
  StickyTimeline.SetTimelineLength(8.0f);
  InitialLifeSpan = 8.0f;
  PrimaryActorTick.bCanEverTick = true;
  SetActorTickEnabled(true);
}

void AStickyProjectile::BeginPlay()
{
  Super::BeginPlay();

	if (StickyTimelineCurve != nullptr) {
	  FOnTimelineFloatStatic StickyTimelineCallBack;
	  StickyTimelineCallBack.BindUFunction(this, TEXT("ModulateColor"));
	  StickyTimeline.AddInterpFloat(StickyTimelineCurve, StickyTimelineCallBack);
	}
}

void AStickyProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  StickyTimeline.TickTimeline(DeltaTime);
}
void AStickyProjectile::ModulateColor()
{
  float TimelinePosition = StickyTimeline.GetPlaybackPosition();
  auto Fmodifier = StickyTimelineCurve->GetFloatValue(TimelinePosition);
  BaseColor = BaseColor - FLinearColor(Fmodifier, Fmodifier, Fmodifier, 0.0);
  MeshMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);
  MeshComponentPtr->SetMaterial(0, MeshMaterialInstance);
  // UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"CURVE: TICK!\""));
}

void AStickyProjectile::OnHit(
  UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and attach projectile if we hit a player/character that is not the object causing the hit
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
	  OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	  auto BaseCharacter = StaticCast<ABaseShooter*>(OtherActor);
		if (BaseCharacter == nullptr) {
		  return;
		}
		if (BaseCharacter->StaticClass() == ABaseShooter::StaticClass()) {
		  UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"HIT YOU!\""));
		  // Stick to other character skeleton here, at impact point.
		  // AttachToComponent(BaseCharacter->GetMeshPtr(), FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
		  StickyTimeline.SetPlayRate(2.0f);
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
}
