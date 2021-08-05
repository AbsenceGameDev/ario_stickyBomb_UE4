// Ario Amin - 2021/08

#pragma once

#include "Actors/StickyBaseActor.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Interfaces/InteractionUOI.h"

#include <Templates/SharedPointer.h>

#include "StickyPickupContainer.generated.h"

/**
 * @author  Ario Amin
 * @file    Actors/StickyPickupContainer.h
 * @class   AStickyPickupContainer
 * @brief   A simple actor which holds a static mesh container
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyPickupContainer : public AStickyBaseActor
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new AStickyPickupContainer object
	 *
	 */
	AStickyPickupContainer();

	protected:
	/* ============================ */
	/* Inherited Methods: Overrides */
	virtual void Tick(float DeltaTime) final;
	virtual void BeginPlay() final;

	public:
	/* ============================== */
	/* Interface Methods: Interaction */

	/**
	 * @brief   Interact Item, End
	 */
	virtual void EndInteractItem() override;

	/**
	 * @brief   Interact Item, Start
	 */
	virtual void TryInteractItem() override;

	protected:
	/* ============================ */
	/* Protected Fields: Components */

	UPROPERTY(VisibleDefaultsOnly, Category = ContainerActors)
	TArray<UStaticMeshComponent*> MeshComponentArray;

	private:
	/* ======================================= */
	/* Private Methods: Component Initializers */

	/**
	 * @brief   Set the Collision Responses object
	 *
	 */
	void SetCollisionResponses();

	/**
	 * @brief   Generate Static Mesh Component
	 * @details Generates cube of spheres, surround with wall type static meshes.
	 */
	void ConstructStaticMeshComponents();

	/* ================================ */
	/* Private Fields: Basic Properties */
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;
};
