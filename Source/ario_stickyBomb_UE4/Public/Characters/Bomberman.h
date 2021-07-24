// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseShooter.h"
#include "CoreMinimal.h"

#include "Bomberman.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UStickyGunSkeletalComp;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS()
class ARIO_STICKYBOMB_UE4_API ABomberman : public ABaseShooter
{
  GENERATED_BODY()

  public:
  ABomberman();

  protected:
  virtual void BeginPlay();

  virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};
