// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Bomberman.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StickyGunSkeletalComp.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"	   // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
ABomberman::ABomberman()
{
}

void ABomberman::BeginPlay()
{
  // Call the base class
  Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABomberman::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
  // set up gameplay key bindings
  check(PlayerInputComponent);

  // Bind jump events
  PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
  PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

  // Bind fire event
  PlayerInputComponent->BindAction("Fire", IE_Pressed, StickyGun, &UStickyGunSkeletalComp::OnFire);

  // Bind movement events
  PlayerInputComponent->BindAxis("MoveForward", this, &ABomberman::MoveForward);
  PlayerInputComponent->BindAxis("MoveRight", this, &ABomberman::MoveRight);

  // We have 2 versions of the rotation bindings to handle different kinds of devices differently
  // "turn" handles devices that provide an absolute delta, such as a mouse.
  // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
  PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
  PlayerInputComponent->BindAxis("TurnRate", this, &ABomberman::TurnAtRate);
  PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
  PlayerInputComponent->BindAxis("LookUpRate", this, &ABomberman::LookUpAtRate);
}