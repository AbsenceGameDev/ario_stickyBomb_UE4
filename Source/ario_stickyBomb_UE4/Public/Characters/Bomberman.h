// Ario Amin - 2021/08

#pragma once

#include "Characters/BaseShooter.h"
#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include "Bomberman.generated.h"

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
