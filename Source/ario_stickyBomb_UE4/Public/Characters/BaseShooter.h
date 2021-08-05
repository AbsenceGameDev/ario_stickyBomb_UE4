// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Interfaces/InteractionUOI.h"

#include <Delegates/Delegate.h>
#include <GameFramework/Character.h>

#include "BaseShooter.generated.h"

/**
 * @author  Ario Amin
 * @file    Characters/ABaseShooter.h
 * @class   ABomberman
 * @brief   Inherits from ACharacter & IInteractionUOI
 * @details Derived from ACharacter and implements a StickyGunSkeletalComponent, HealthComponent and a AmmoComponent
 * @todo    Currently this class is fairly bloated.
 *          1. Move some of the functions of ABaseShooter into Bomberman, or rather
 *          rewrite some as virtual and implement them in child classes such as ABomberMan.
 *          2. Move methods such as those regarding to input setup to the player controller class
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API ABaseShooter : public ACharacter, public IInteractionUOI
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new ABaseShooter object
	 *
	 */
	ABaseShooter();

	protected:
	/* ============================ */
	/** Inherited Methods: Overrides */

	/**
	 * @brief Triggers BeginPlay
	 *
	 */
	virtual void BeginPlay();

	/**
	 * @brief
	 *
	 * @param InputComponent
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/**
	 * @brief
	 *
	 * @param DamageAmount
	 * @param DamageEvent
	 * @param EventInstigator
	 * @param DamageCauser
	 * @return float
	 */
	virtual float TakeDamage(
		float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) final;

	public:
	/* ================================== */
	/** Interface Methods: IInteractionOUI */

	/**
	 * @brief Interact Item, Start
	 */
	virtual void TryInteractItem() override;

	/**
	 * @brief Interact Item, End
	 */
	virtual void EndInteractItem() override;

	/* ======================= */
	/* Public Methods: Getters */

	/**
	 * @brief Get the Sticky Gun object
	 *
	 * @return UStickyGunSkeletalComp*
	 */
	UStickyGunSkeletalComp* GetStickyGun();

	/**
	 * @brief Get the Char Mesh object
	 *
	 * @return USkeletalMeshComponent*
	 */
	USkeletalMeshComponent* GetCharMesh();

	/**
	 * @brief Get the Health Comp object
	 *
	 * @return UHealthComp*
	 */
	UHealthComp* GetHealthComp();

	/**
	 * @brief Get the Ammo Comp object
	 *
	 * @return UAmmoComp*
	 */
	UAmmoComp* GetAmmoComp();

	/**
	 * @brief Get the First Person Camera Component object
	 *
	 * @return UCameraComponent*
	 */
	UCameraComponent* GetFirstPersonCameraComponent();

	/* ================================ */
	/* Public Methods: Client Interface */

	/**
	 * @brief
	 *
	 */
	void TryStartFire();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTriggerRagdoll();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUndoRagdoll();

	/* ====================== */
	/* Public Methods: UI/HUD */

	/**
	 * @brief
	 *
	 * @param LocalAmmoUpdate
	 */
	void TriggerPlayerStateAmmo(int LocalAmmoUpdate);

	/* =========================== */
	/* Public Fields: Rates/Limits */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate; /** In deg/sec. Other scaling may affect final turn rate. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate; /** In deg/sec. Other scaling may affect final rate. */

	protected:
	/* ================================= */
	/* Protected Methods: Server/Client */

	/**
	 * @brief
	 *
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUndoRagdoll();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTriggerRagdoll();

	/**
	 * @brief
	 *
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTryInteractItem();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteractItem();

	UFUNCTION(Server, Reliable)
	void ServerTakeDamage(
		AActor* ThisActor, float DamageAmount, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser);

	/* ================================== */
	/* Protected Methods: Component Setup */
	void InitSkeletalBody();
	void InitCamera();
	void InitActorComponents();
	void SetupStickyGun();
	void SetupCollision();

	/* ========================= */
	/* Protected Methods: Inputs */
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

	/**
	 * @brief Create a New Axis Mapping object
	 *
	 * @param DesiredAxisName
	 * @param DesiredAxisKey
	 * @todo Move to a controller class perhaps?
	 */
	void CreateNewAxisMapping(FName DesiredAxisName, FKey DesiredAxisKey);

	/**
	 * @brief Create a New Action Mapping object
	 *
	 * @param DesiredActionName
	 * @param DesiredActionKey
	 * @todo Move to a controller class perhaps?
	 */
	void CreateNewActionMapping(FName DesiredActionName, FKey DesiredActionKey);

	/* ============================ */
	/* Protected Fields: Components */

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* MeshPtr = nullptr; /** SkelMesh: 1st person view arms */

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStickyGunSkeletalComp* StickyGun = nullptr; /** SkelMesh: Skeletal Gun mesh */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent = nullptr; /** First person camera */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComp* HealthComponent = nullptr; /** Simple health component */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UAmmoComp* AmmoComp = nullptr; /** Simpole ammo component */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UStickyLinetraceComp* LinetraceComp = nullptr; /** Interact detection linetrace comp */

	/* ================================== */
	/* Protected Fields: Basic properties */
	bool bIsRagdoll = false;
};
