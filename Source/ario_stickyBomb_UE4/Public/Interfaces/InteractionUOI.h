// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <UObject/Interface.h>

#include "InteractionUOI.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionUOI : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ARIO_STICKYBOMB_UE4_API IInteractionUOI
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	UFUNCTION()
	virtual void BeginInteractItem() = 0;

	UFUNCTION()
	virtual void EndInteractItem() = 0;
};
