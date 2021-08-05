// Ario Amin - 2021/08

#pragma once

#include "Actors/StickyBaseActor.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Interfaces/InteractionUOI.h"

#include <Templates/SharedPointer.h>

#include "StickyPickup.generated.h"

/**
 * @author  Ario Amin
 * @file    Actors/StickyPickup.h
 * @class   AStickyPickup
 * @brief   A simple pickup actor
 * @details Has rudimentary pickup logic which sends it to the players ammo component
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyPickup : public AStickyBaseActor
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new AStickyPickup object
	 *
	 */
	AStickyPickup();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */
	virtual void Tick(float DeltaTime) final;
	virtual void BeginPlay() final;

	public:
	/* ============================== */
	/** Interface Methods: Interaction */

	/**
	 * @brief Interact Item, End
	 */
	virtual void EndInteractItem() override;

	/**
	 * @brief Interact Item, Start
	 */
	virtual void TryInteractItem() override;

	/* ================================ */
	/* Public Methods: Client Interface */

	/**
	 * @brief
	 *
	 * @param OtherActor
	 * @return true
	 * @return false
	 */
	UFUNCTION()
	bool DidPickup(AActor* OtherActor);

	/**
	 * @brief Get the Collision Comp object
	 *
	 * @return USphereComponent*
	 */
	USphereComponent* GetCollisionComp() const;

	protected:
	/* ============================ */
	/* Protected Fields: Components */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr = nullptr;

	private:
	/* ======================================= */
	/* Private Methods: Component Initializers */

	/**
	 * @brief
	 *
	 */
	void ConstructCollisionComponent();

	/**
	 * @brief Set the Collision Responses object
	 *
	 */
	void SetCollisionResponses();

	/**
	 * @brief
	 *
	 */
	void ConstructStaticMeshComponent();

	/* ================================ */
	/* Private Fields: Basic Properties */
	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	UPROPERTY(VisibleDefaultsOnly, Category = Interaction)
	AStickyGameMode* CurrentGameMode = nullptr;
};
