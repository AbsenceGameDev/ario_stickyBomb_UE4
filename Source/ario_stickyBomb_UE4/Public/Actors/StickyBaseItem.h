// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <GameFramework/Actor.h>

#include "StickyBaseItem.generated.h"

UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyBaseItem : public AActor
{
	GENERATED_BODY()

	public:
	AStickyBaseItem();

	protected:
	/** ============================ **/
	/** Inherited Methods: Overrides **/
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	public:
	/** =============================== **/
	/** Public Methods: Getters/Setters **/

	USphereComponent*					GetCollisionComp() const;
	TArray<UActorComponent*>& GetReplicatedComponents();

	/** ============================ **/
	/** Public Methods: Conditionals **/

	UFUNCTION()
	virtual bool DidPickup(AActor* OtherActor)
	{
		// Do nothing
		return false;
	};

	protected:
	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/

	UFUNCTION()
	virtual void OnPickup(ABaseShooter* CallerBaseShooterActor)
	{
		// Do nothing
		return;
	}

	/** ============================ **/
	/** Protected Fields: Components **/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComponentPtr = nullptr;

	/** ========================================= **/
	/** Protected Methods: Component Initializers **/
	void				 ConstructCollisionComponent(FName ComponentString, FName CollisionProfileString);
	virtual void ConstructStaticMeshComponent(FString MeshName, FString MeshPath, FString BaseMaterialName, FString BaseMaterialPath)
	{
		// Do nothing
		return;
	};

	/** ================================== **/
	/** Protected Fields: Basic Properties **/
	UPROPERTY(VisibleDefaultsOnly, Category = Material)
	UMaterialInstanceDynamic* MeshMaterialInstance;

	FLinearColor BaseColor = FLinearColor(0.960784, 0.584314, 0.109804, 1.000000);
};
