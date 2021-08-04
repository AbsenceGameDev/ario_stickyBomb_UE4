// Ario Amin - 2021/08

#include "Components/StickyLinetraceComp.h"

// Actors
#include "Actors/StickyProjectile.h"
#include "Characters/BaseShooter.h"

// Components
#include "Components/AmmoComp.h"

// Helpers
#include "Helpers/Macros.h"

// Interfaces
#include "Interfaces/InteractionUOI.h"

// Engine Frameworks
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UStickyLinetraceComp::UStickyLinetraceComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SetComponentTickInterval(TickInterval);
	PrimaryComponentTick.bCanEverTick					 = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}

// Called when the game starts
void UStickyLinetraceComp::BeginPlay()
{
	Super::BeginPlay();
	WorldPtr		= GetWorld();
	bValidOwner = (BaseChar = StaticCast<ABaseShooter*>(GetOwner())) == NULL ? true : false;
}

// Called every frame
void UStickyLinetraceComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_Authority) {
		FVector StartPosition = GetComponentLocation();
		FVector EndPosition		= StartPosition + (GetForwardVector() * LookAtDistance);
		GetWorld()->LineTraceSingleByChannel(*HitResult, StartPosition, EndPosition, ECC_Visibility);

		if (HitResult != NULL) {
			AStickyProjectile* CastOtherBaseItem = StaticCast<AStickyProjectile*>((HitResult->Actor.Get()));
			if (CastOtherBaseItem == NULL) {
				return;
			}

			if (CastOtherBaseItem->GetClass()->ImplementsInterface(UInteractionUOI::StaticClass())) {
				Cast<IInteractionUOI>(CastOtherBaseItem)->TryInteractItem();

				// Under the presumption that this component will only be used with BaseShooter derived classes
				// @todo TryPickUpRound is too direct way to propagate the interaction to anothe component
				StaticCast<ABaseShooter*>(GetOwner())->GetAmmoComp()->TryPickupRound();
			}
// STICKY_DEBUG is set in Helpers/Macros.h
#ifdef STICKY_DEBUG
			UKismetSystemLibrary::DrawDebugLine(WorldPtr, StartPosition, HitResult->Location, FColor(100, 0, 0), true, 0.1f);
			UKismetSystemLibrary::DrawDebugLine(WorldPtr, StartPosition, HitResult->Location, FColor(100, 0, 0), true, 0.1f);
			UKismetSystemLibrary::DrawDebugSphere(WorldPtr, HitResult->Location, 25.0f, 12, FColor(100, 0, 0), true, 0.1f);
#endif
			return;
		}
#ifdef STICKY_DEBUG
		UKismetSystemLibrary::DrawDebugLine(WorldPtr, StartPosition, EndPosition, FColor(0, 100, 0), true, 0.1f);
		UKismetSystemLibrary::DrawDebugSphere(WorldPtr, EndPosition, 25.0f, 12, FColor(0, 100, 0), true, 0.1f);
#endif
	}
}