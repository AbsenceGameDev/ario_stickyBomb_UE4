// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

#include <UObject/Interface.h>

#include "InteractionUOI.generated.h"

/**
 * @author Ario Amin  @file Interfaces/InteractionUOI.h
 * @class UInteractionUOI
 */
UINTERFACE(MinimalAPI)
class UInteractionUOI : public UInterface
{
	GENERATED_BODY()
};

/**
 * @author Ario Amin  @file Interfaces/InteractionUOI.h
 * @class IInteractionUOI
 * @brief Interaction Interface
 * @details Simple Interaction Interface for managing pickups, could be more fleshed out if needed.
 */
class ARIO_STICKYBOMB_UE4_API IInteractionUOI
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Pure virtual, implement in child-classes
	 *
	 */
	UFUNCTION()
	virtual void TryInteractItem() = 0;

	/**
	 * @brief Pure virtual, implement in child-classes
	 *
	 */
	UFUNCTION()
	virtual void EndInteractItem() = 0;
};
