// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "AmmoWidget.generated.h"

// Forward decl.
class UTextBlock;

/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API UAmmoWidget : public UUserWidget
{
  GENERATED_BODY()

  public:
  UAmmoWidget(const FObjectInitializer& ObjectInitializer);

  void InitWidget();

  virtual void NativeConstruct() final;

  void UpdateAmmoCount(int AmmoCount);

  void AppendReloadPrompt();

  // Assumes previous knowledge of object validity
  void ShowWidget();
  void HideWidget();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* AmmoCountTextBlock;

  private:
  FText TextResource = FText::FromString("AMMO: N/A");
};