// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Components/WidgetComponent.h>
#include <GameFramework/HUD.h>

#include "StickyHUD.generated.h"

/** Forward decl. @todo Have not made this widget yet, mid-priority */
class SAmmoWidget;
class SHealthWidget;

/** Forward decl. */
class SKillWidget;
class SSlideInText;
class SKillContentContainer;

/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyHUD : public AHUD
{
  GENERATED_BODY()

  public:
  AStickyHUD();

  virtual void Tick(float DeltaTime) final;
  virtual void DrawHUD() final;

  // UFUNCTION()
  // void UpdateAmmoCount(int AmmoCount);

  //  UFUNCTION()
  //  void AppendReloadPrompt();

  UFUNCTION(BlueprintCallable, Category = "UIActions")
  void UpdateTotalKills();

  void ToggleGameMenu();
  void AddKillToWidget(FString Kill);

  protected:
  TSharedPtr<SKillWidget> TotalKillsWidget;
  TSharedPtr<SSlideInText> OverlayMenu;
  TSharedPtr<SKillContentContainer> KillList;

  bool bIsTitleVisible;
  bool bisKillWidgetInitialized;
  bool bisOverlayMenuVisible;

  void InitializeKillOverlayWidget();
  void InitializeTotalKillsWidget();

  virtual void BeginPlay() final;
  virtual void PostInitializeComponents() override;
};
