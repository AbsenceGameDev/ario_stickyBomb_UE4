// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/StickyProjectile.h"

// General
#include "Characters/BaseShooter.h"
#include "Interfaces/InteractionUOI.h"
#include "StickyGameMode.h"
#include "StickyPlayerState.h"

// Component
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "Components/StickyGunSkeletalComp.h"

// Helpers
#include "Helpers/CollisionChannels.h"
#include "Helpers/Macros.h"

// Engine Frameworks
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

// Engine Helpers
#include <UObject/ConstructorHelpers.h>

// Engine Components
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Engine Classes/Types
#include <GameFramework/DamageType.h>
#include <Materials/MaterialInstanceDynamic.h>

AStickyProjectile::AStickyProjectile()
{
	ConstructCollisionComponent();
	ConstructProjectileMovementComponent();
	ConstructStaticMeshComponent();
	StickyTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("StickyTimelineComp"));

	// Die after 'MaxCurrentLifetime' seconds by default
	InitialLifeSpan												 = MaxCurrentLifetime;
	PrimaryActorTick.bCanEverTick					 = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval					 = 1.0f;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);

	// Tick will be disabled at start, enabled when registering hit, disabled after pickup
	// SetActorTickEnabled(false);

	InterpTimelineEvent.BindUFunction(this, FName{TEXT("ModulateColor")});
	TimelineFinishedEvent.BindUFunction(this, FName{TEXT("TriggerExplosionFX")});
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyProjectile::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStickyProjectile::BeginPlay()
{
	Super::BeginPlay();
	CurrentGameMode = StaticCast<AStickyGameMode*>(UGameplayStatics::GetGameMode(this));

	if (IsValid(StickyTimelineCurve)) {
		StickyTimelineComp->AddInterpFloat(GetCurve(), InterpTimelineEvent);
		StickyTimelineComp->SetTimelineFinishedFunc(TimelineFinishedEvent);
		StickyTimelineComp->PlayFromStart();
	}
}

void AStickyProjectile::LifeSpanExpired()
{
	Super::LifeSpanExpired();
	OnExplode();
}

void AStickyProjectile::TryInteractItem()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(false);
	Destroy();
}
void AStickyProjectile::EndInteractItem() {}

/** =============================== **/
/** Public Methods: Getters/Setters **/

float													AStickyProjectile::GetMaxLifetime() const { return MaxCurrentLifetime; };
USphereComponent*							AStickyProjectile::GetCollisionComp() const { return CollisionComp; }
UProjectileMovementComponent* AStickyProjectile::GetProjectileMovement() const { return ProjectileMovement; }
UTimelineComponent*						AStickyProjectile::GetTimelineComp() const { return StickyTimelineComp; }
TArray<UActorComponent*>&			AStickyProjectile::GetReplicatedComponents() { return ReplicatedComponents; };

float AStickyProjectile::GetDamageRadius() const { return DamageRadius; }
void	AStickyProjectile::SetDamageRadius(float InRadius) { DamageRadius = InRadius; }

float AStickyProjectile::GetDamageAmount() const { return DamageValue; }
void	AStickyProjectile::SetDamageAmount(float InDamage) { DamageValue = InDamage; }

void				 AStickyProjectile::SetMaxPossibleLifetime(float MaxLifetime) { MaxPossibleLifetime = MaxLifetime; }
void				 AStickyProjectile::SetCurve(UCurveFloat* InCurve) { StickyTimelineCurve = InCurve; }
UCurveFloat* AStickyProjectile::GetCurve() { return StickyTimelineCurve; }
// void AStickyProjectile::MulticastSetCurve(UCurveFloat InCurve) { *StickyTimelineCurve = InCurve; }

/** ============================ **/
/** Public Methods: Conditionals **/
bool AStickyProjectile::DidPickup(AActor* OtherActor)
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

/** ================================= **/
/** Protected Methods: Actions/Events **/
void AStickyProjectile::OnHit(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
		CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Ignore);
		OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
		USkeletalMeshComponent* CastOtherSkelMesh		 = StaticCast<USkeletalMeshComponent*>(OtherComp);
		ABaseShooter*						CastOtherBaseShooter = StaticCast<ABaseShooter*>(CastOtherSkelMesh->GetOwner());
		ABaseShooter*						CastOwnerBaseShooter = StaticCast<ABaseShooter*>(this->GetOwner());

		// Don't attach if it doesn't have a owner or if it doesn't hit another ABaseShooter derived char
		if (
			CastOtherBaseShooter == NULL || CastOwnerBaseShooter == NULL || CastOtherSkelMesh == NULL ||
			(CastOwnerBaseShooter == CastOtherBaseShooter)) {
			CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);
			return;
		}

		if (Hit.BoneName.ToString() == FString("None")) {
			CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);
			return;
		}

		ProjectileMovement->bShouldBounce = false;
		ProjectileMovement->ClearPendingForce(true);
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->ProjectileGravityScale = 0;
		CollisionComp->SetEnableGravity(false);
		MeshComponentPtr->SetEnableGravity(false);
		AttachedToActor = CastOtherBaseShooter;

		FAttachmentTransformRules AttachRules =
			FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
		AttachToActor(AttachedToActor, AttachRules);
		return;
	}
}
void AStickyProjectile::OnExplode()
{
	if (GetLocalRole() == ROLE_Authority) {
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(), DamageValue, GetActorLocation(), DamageRadius, UDamageType::StaticClass(), TArray<AActor*>(), this,
			this->GetInstigatorController(), true, ECC_Visibility);
	}
	TriggerExplosionFX();
	Destroy();
}

void AStickyProjectile::OnPickup(ABaseShooter* CallerBaseShooterActor)
{
	/*
	CallerBaseShooterActor->->SetCanPickup(false);
	CallerBaseShooterActor->GetAmmoComp()->TryPickupRound();
	Destroy();
	Call Ammo Component, add Ammo */

	return;
}

/** ========================== **/
/** Protected Methods: VFX/SFX **/

void AStickyProjectile::ModulateColor_Implementation(const float InterpValue)
{
	// Timeline position normalized in regards to max lifetime
	float TimelinePosition = StickyTimelineComp->GetPlaybackPosition() / MaxPossibleLifetime;
	auto	ColorAdditive		 = (FLinearColor(InterpValue, InterpValue, InterpValue, 0.0f));
	BaseColor += ColorAdditive;

	auto DynamicMaterialInstance = MeshComponentPtr->CreateDynamicMaterialInstance(0);
	DynamicMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);
	UE_LOG(LogTemp, Log, TEXT("git commit -S -m \"TimelinePosition: %f -- InterpValue: %f \""), TimelinePosition, InterpValue);
	UE_LOG(
		LogTemp, Log, TEXT("git commit -S -m \"COLORADD.R: %f \n-- COLORADD.G: %f \n-- COLORADD.B: %f \n\""), ColorAdditive.R,
		ColorAdditive.G, ColorAdditive.B);
}

void AStickyProjectile::TriggerExplosionFX()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Event Called."));

	// UParticleSystemComponent* TempComponent = UGameplayStatics::SpawnEmitterAttached(
	// 	UParticleSystem * EmitterTemplate, USceneComponent * AttachToComponent, FName AttachPointName, FVector Location,
	// 	FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy);
	// Kill or Damage actor, trigger HealthComponent OnDamage
	// at the very least on any BaseShooter derived character within DamageRadius
}

/** ======================================= **/
/** Private Methods: Component Initializers **/

void AStickyProjectile::ConstructCollisionComponent()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentHit.AddDynamic(this, &AStickyProjectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	CollisionComp->SetCollisionObjectType(ECC_StickyProjectile);
	SetCollisionResponses();

	// Set as root component
	RootComponent = CollisionComp;
}

void AStickyProjectile::SetCollisionResponses()
{
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_StickyGun, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);
}

void AStickyProjectile::ConstructProjectileMovementComponent()
{
	ProjectileMovement															= CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent						= CollisionComp;
	ProjectileMovement->ProjectileGravityScale			= 0.2f;
	ProjectileMovement->InitialSpeed								= 3000.f;
	ProjectileMovement->MaxSpeed										= 3000.f;
	ProjectileMovement->bRotationFollowsVelocity		= true;
	ProjectileMovement->bShouldBounce								= true;
	ProjectileMovement->bAutoUpdateTickRegistration = true;
	// ProjectileMovement->OnProjectileBounce.AddDynamic(this,&AStickyProjectile::OnBounce);
}

// AStickyProjectile::OnBounce()

void AStickyProjectile::ConstructStaticMeshComponent()
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

/*
 */
void AStickyProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStickyProjectile, AttachedToActor);
}