/**
 * @author  Ario Amin
 * @file    Actors/StickyBaseActor.h
 * @class   AStickyBaseActor
 * @brief   A simple shell of a class to be expanded upon.
 */
#include "Actors/StickyBaseActor.h"

AStickyBaseActor::AStickyBaseActor()
{
	// Die after 'MaxCurrentLifetime' seconds by default
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;		 // Correct procedure for pre-init actors
	SetActorHiddenInGame(false);
	SetReplicateMovement(true);

	SetActorTickEnabled(false);
}

void AStickyBaseActor::TryInteractItem()
{
	// Do nothing yet
	return;
}
void AStickyBaseActor::EndInteractItem()
{
	// Do nothing yet
	return;
}