// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/InteractionUOI.h"

#include <GameFramework/Actor.h>

#include "StickyBaseActor.generated.h"

/**
 * @author  Ario Amin
 * @file    Actors/StickyBaseActor.h
 * @class   AStickyBaseActor
 * @brief   A simple shell of a class to be expanded upon.
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyBaseActor : public AActor, public IInteractionUOI
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new AStickyBaseActor object
	 *
	 */
	AStickyBaseActor();

	/* ============================== */
	/* Interface Methods: Interaction */

	/**
	 * @brief   Interact Item, End
	 */
	virtual void TryInteractItem() override;

	/**
	 * @brief   Interact Item, Start
	 */
	virtual void EndInteractItem() override;
};
