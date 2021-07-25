// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BaseShooter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UStickyGunSkeletalComp;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class UHealthComp;

UCLASS()
class ARIO_STICKYBOMB_UE4_API ABaseShooter : public ACharacter
{
  GENERATED_BODY()

  protected:
  /** Pawn mesh: 1st person view (arms; seen only by self) */
  UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
  USkeletalMeshComponent* MeshPtr;

  /** Gun mesh: 1st person view (seen only by self) */
  UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
  UStickyGunSkeletalComp* StickyGun;

  /** First person camera */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  UCameraComponent* FirstPersonCameraComponent;

  /** Generic health component */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  UHealthComp* HealthComponent;

  public:
  ABaseShooter();

  protected:
  virtual void BeginPlay();

  public:
  /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
  float BaseTurnRate;

  /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
  float BaseLookUpRate;

  protected:
  /** Handles moving forward/backward */
  void MoveForward(float Val);

  /** Handles stafing movement, left and right */
  void MoveRight(float Val);

  /**
   * Called via input to turn at a given rate.
   * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
   */
  void TurnAtRate(float Rate);

  /**
   * Called via input to turn look up/down at a given rate.
   * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
   */
  void LookUpAtRate(float Rate);

  protected:
  // APawn interface
  virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
  // End of APawn interface

  public:
  void FireGunEffects(UStickyGunSkeletalComp* StickyGunPtr);
  void InitSkeletalBody();
  void InitCamera();
  void InitActorComponents();
  void SetupStickyGun();
  UStickyGunSkeletalComp* GetStickyGunPtr();

  /** Returns Mesh1P subobject **/
  USkeletalMeshComponent* GetMeshPtr() const
  {
	return MeshPtr;
  }
  /** Returns FirstPersonCameraComponent subobject **/
  UCameraComponent* GetFirstPersonCameraComponent() const
  {
	return FirstPersonCameraComponent;
  }
};
