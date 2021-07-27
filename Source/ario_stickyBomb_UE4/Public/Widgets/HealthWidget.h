// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "HealthWidget.generated.h"


/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API UHealthWidget : public UUserWidget
{
  GENERATED_BODY()

  public:
  UHealthWidget(const FObjectInitializer& ObjectInitializer);
};
