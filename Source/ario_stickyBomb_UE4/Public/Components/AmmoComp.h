// Ario Amin - 2021/08

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Helpers/Macros.h"

#include <Engine/EngineTypes.h>

#include "AmmoComp.generated.h"

// OnAmmoChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnAmmoChangedSignature, UAmmoComp*, OwningAmmoComp, int, Ammo, int, AmmoDelta, AController*, InstigatedBy);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARIO_STICKYBOMB_UE4_API UAmmoComp : public UActorComponent
{
	GENERATED_BODY()

	public:
	UAmmoComp();

	protected:
	/** ============================ **/
	/** Inherited Methods: Overrides **/
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	public:
	/** ================================ **/
	/** Public Methods: Client interface **/
	UFUNCTION()
	void TryFire();

	UFUNCTION()
	void TryPickupRound();

	/** =============================== **/
	/** Public Methods: Inlined Getters **/
	float GetAmmo() const { return AmmoCount; }
	bool	IsEmpty() const { return bIsEmpty; }
	bool	IsFullClip() const { return AmmoCount >= MaxAmmo; }

	/** =============================== **/
	/** Public Fields: Events/Delegates **/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	protected:
	/** ======================================== **/
	/** Protected Methods: Client/Server actions **/
	UFUNCTION()
	void OnRep_Ammo(int PrevAmmo);

	UFUNCTION()
	void OnFire();

	UFUNCTION()
	void OnPickupRound();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(); /** Send request to host server, withValidation **/

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnPickupRound(); /** Send request to host server, withValidation **/

	/** ======================================== **/
	/** Protected Fields: Ammo **/
	bool bIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoComp")
	int MaxAmmo = DEFAULT_STICKY_GUN_MAX;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "AmmoComp")
	int AmmoCount = MaxAmmo;
};
