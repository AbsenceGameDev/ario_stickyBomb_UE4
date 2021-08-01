// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/StickyProjectile.h"

#include "Characters/BaseShooter.h"
#include "Components/AmmoComp.h"
#include "Components/HealthComp.h"
#include "Components/StickyGunSkeletalComp.h"
#include "Helpers/CollisionChannels.h"
#include "Helpers/Macros.h"
#include "Interfaces/InteractionUOI.h"

#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

AStickyProjectile::AStickyProjectile()
{
	ConstructCollisionComponent();
	ConstructProjectileMovementComponent();
	ConstructStaticMeshComponent();
	StickyTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("StickyTimelineComp"));

	// Die after 'MaxCurrentLifetime' seconds by default
	InitialLifeSpan								= MaxCurrentLifetime * 2;
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);

	SetActorTickEnabled(true);
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if (StickyTimelineComp != nullptr) {
	// 	StickyTimelineComp->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	// }
}

void AStickyProjectile::BeginPlay() { Super::BeginPlay(); }

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
void AStickyProjectile::PlayTimeline() {}

void AStickyProjectile::OnHit(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() != ROLE_Authority) {
		return;
	}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
		SetActorEnableCollision(false);
		OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
		USkeletalMeshComponent* CastOtherSkelMesh		 = StaticCast<USkeletalMeshComponent*>(OtherComp);
		ABaseShooter*						CastOtherBaseShooter = StaticCast<ABaseShooter*>(CastOtherSkelMesh->GetOwner());
		ABaseShooter*						CastOwnerBaseShooter = StaticCast<ABaseShooter*>(this->GetOwner());

		// Don't attach if it doesn't have a owner or if it doesn't hit another ABaseShooter derived char
		if (
			CastOtherBaseShooter == NULL || CastOwnerBaseShooter == NULL || CastOtherSkelMesh == NULL ||
			(CastOwnerBaseShooter == CastOtherBaseShooter)) {
			SetActorEnableCollision(true);
			return;
		}

		FAttachmentTransformRules AttachRules =
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, true);
		// bool bLocalIsDead = CastOtherBaseShooter->GetHealthComp()->IsDead();

		if (Hit.BoneName.ToString() == FString("None")) {
			SetActorEnableCollision(true);
			return;
		}
		SetActorTickEnabled(false);
		ProjectileMovement->ClearPendingForce(true);
		ProjectileMovement->StopSimulating(Hit);
		// Stick to other character skeleton here, at impact point.
		AttachToActor(CastOtherBaseShooter, AttachRules, Hit.BoneName);
		return;

		// StickyTimelineComp->SetPlayRate(2.0f);
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

void AStickyProjectile::ModulateColor(const float InterpValue)
{
	static int TestModulator = 1;		 // Only sets once per object
	TestModulator *= -1;						 // Flip value

	float TimelinePosition = StickyTimelineComp->GetPlaybackPosition() / MaxPossibleLifetime;
	auto	Fmodifier				 = StickyTimelineCurve->GetFloatValue(TimelinePosition);
	auto	ValMod					 = TestModulator + Fmodifier;
	auto	ColorAdditive		 = (FLinearColor(ValMod, ValMod, ValMod, 0.0f));
	BaseColor += ColorAdditive;
	// MeshMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);

	//
	// MeshComponentPtr->GetMaterial(0)
	// ->GetMaterialResource(ERHIFeatureLevel::Type::Num, EMaterialQualityLevel::Type::Num)

	auto DynamicMaterialInstance = MeshComponentPtr->CreateDynamicMaterialInstance(0);
	DynamicMaterialInstance->SetVectorParameterValue("DiffuseColor", BaseColor);
	UE_LOG(
		LogTemp, Log, TEXT("git commit -S -m \"TimelinePosition: %f -- Fmodifier: %f -- ValMod: %f \""), TimelinePosition, Fmodifier,
		ValMod);
	UE_LOG(
		LogTemp, Log, TEXT("git commit -S -m \"COLOR.R: %f -- COLOR.G: %f -- COLOR.B: %f \""), ColorAdditive.R, ColorAdditive.G,
		ColorAdditive.B);
}

void AStickyProjectile::TriggerExplosionFX()
{
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

	// SetChannel
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_StickyGun, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_CharacterMesh, ECollisionResponse::ECR_Block);

	// Set as root component
	RootComponent = CollisionComp;
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
}

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
}