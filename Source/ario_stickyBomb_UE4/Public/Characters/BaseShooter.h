// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <GameFramework/Character.h>

#include "BaseShooter.generated.h"

UCLASS()
class ARIO_STICKYBOMB_UE4_API ABaseShooter : public ACharacter
{
	GENERATED_BODY()

	public:
	ABaseShooter();

	/** ============================ **/
	/** Inherited Methods: Overrides **/
	protected:
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** ======================= **/
	/** Public Methods: Getters **/
	public:
	UStickyGunSkeletalComp* GetStickyGunPtr();
	USkeletalMeshComponent* GetMeshPtr();
	UHealthComp*						GetHealthCompPtr();
	UAmmoComp*							GetAmmoCompPtr();
	UCameraComponent*				GetFirstPersonCameraComponent();

	/** ======================= **/
	/** Public Methods: Actions **/
	void TryStartFire();
	void TryPickupRound();

	/** ======================= **/
	/** Public Methods: VFX/SFX **/
	void FireGunEffects();
	void TriggerPlayerStateAmmo(int LocalAmmoUpdate);
	/** =========================== **/
	/** Public Fields: Rates/Limits **/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate; /** In deg/sec. Other scaling may affect final turn rate. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate; /** In deg/sec. Other scaling may affect final rate. */

	/** ================================== **/
	/** Protected Methods: Component Setup **/
	protected:
	void InitSkeletalBody();
	void InitCamera();
	void InitActorComponents();
	void SetupStickyGun();

	/** ========================= **/
	/** Protected Methods: Inputs **/
	void MoveRight(float Val);
	void MoveForward(float Val);

	/**
	 * @brief Turn Rate
	 * @details Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * @brief LookUp Rate
	 * @details Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** ============================ **/
	/** Protected Fields: Components **/

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* MeshPtr; /** SkelMesh: 1st person view arms */

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStickyGunSkeletalComp* StickyGun; /** SkelMesh: Skeletal Gun mesh */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent; /** First person camera */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComp* HealthComponent; /** Generic health component */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UAmmoComp* AmmoComp; /** Generic ammo component */
};
