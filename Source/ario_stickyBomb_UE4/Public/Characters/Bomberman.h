// Ario Amin - 2021/08

#pragma once

#include "Characters/BaseShooter.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include "Bomberman.generated.h"

/**
 * @author  Ario Amin
 * @file    Characters/ABomberman.h
 * @class   ABomberman
 * @brief   Inherits from ABaseShooter
 * @details
 * @todo    Move some of the functions of ABaseShooter into Bomberman, or rather
 *          rewrite some as virtual and implement them in child classes such as ABomberMan
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API ABomberman : public ABaseShooter
{
	GENERATED_BODY()

	public:
	/**
	 * @brief Construct a new ABomberman object
	 *
	 */
	ABomberman();

	protected:
	/**
	 * @brief
	 *
	 */
	virtual void BeginPlay();

	/**
	 * @brief
	 *
	 * @param InputComponent
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};
