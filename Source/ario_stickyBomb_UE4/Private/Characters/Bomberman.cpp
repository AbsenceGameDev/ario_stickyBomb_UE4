/**
 * @author  Ario Amin
 * @file    Characters/ABomberman.cpp
 * @class   ABomberman
 * @brief   Inherits from ABaseShooter
 * @details Currently it only sets up the key_bindings
 * @todo    Move some of the functions of ABaseShooter into Bomberman, or rather
 *          rewrite some as virtual and implement them in child classes such as ABomberMan
 **/
#include "Characters/Bomberman.h"

// Components
#include "Components/StickyGunSkeletalComp.h"
#include "Interfaces/InteractionUOI.h"

// Engine Components
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <MotionControllerComponent.h>

// Engine Framework
#include <Animation/AnimInstance.h>
#include <GameFramework/InputSettings.h>
#include <GameFramework/PlayerInput.h>
#include <HeadMountedDisplayFunctionLibrary.h>
#include <XRMotionControllerBase.h>		 // for FXRMotionControllerBase::RightHandSourceId

// Engine Helpers
#include <Kismet/GameplayStatics.h>
#include <UObject/UObjectGlobals.h>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
ABomberman::ABomberman() {}

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

	// Implemented in parent-class
	CreateNewActionMapping(FName("TryInteract"), FKey(FName("F")));

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABomberman::TryStartFire);

	// Bind interact event
	PlayerInputComponent->BindAction("TryInteract", IE_Pressed, this, &ABomberman::TryInteractItem);
	PlayerInputComponent->BindAction("TryInteract", IE_Released, this, &ABomberman::EndInteractItem);

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
