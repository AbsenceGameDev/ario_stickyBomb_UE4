// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Components/WidgetComponent.h>
#include <GameFramework/HUD.h>

#include "StickyHUD.generated.h"

#define MAKETEXT(InString) .Text(FText::FromString(InString))

/**
 *
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyHUD : public AHUD
{
	GENERATED_BODY()

	public:
	AStickyHUD();

	/** ============================ **/
	/** Inherited Methods: Overrides **/
	protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() final;

	public:
	virtual void Tick(float DeltaTime) final;
	virtual void DrawHUD() final;

	/** ================================ **/
	/** Public Methods: Client interface **/
	UFUNCTION(BlueprintCallable, Category = "UIActions")
	void UpdateTotalKills();

	/**
	 * @brief Reflect value to textbox
	 * @details Reflects value from ABaseShooter->StickyGun->AmmoComp->GetAmmoCount(),e
	 **/
	UFUNCTION(BlueprintCallable, Category = "UIActions")
	void UpdateAmmo();

	void ToggleGameMenu();
	// void AppendReloadPrompt();
	void AddKillToWidget(FString Kill);

	protected:
	/** =============================== **/
	/** Protected Methods: Init Widgets **/
	void InitializeKillOverlayWidget();
	void InitializeTotalKillsWidget();
	void InitializeAmmoWidget();

	/** ========================= **/
	/** Protected Fields: Widgets **/
	TSharedPtr<SKillWidget>						TotalKillsWidget;
	TSharedPtr<SAmmoWidget>						AmmoWidget;
	TSharedPtr<SSlideInText>					OverlayMenu;
	TSharedPtr<SKillContentContainer> KillList;
	TSharedPtr<AStickyHUD>						StickyHUD;

	/** ================================== **/
	/** Protected Fields: Basic properties **/
	bool bIsTitleVisible;
	bool bIsKillWidgetInitialized;
	bool bIsAmmoWidgetInitialized;
	bool bIsOverlayMenuVisible;
};
