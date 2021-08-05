// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Helpers/Macros.h"

#include <Components/WidgetComponent.h>
#include <GameFramework/HUD.h>

#include "StickyHUD.generated.h"

/**
 * @author Ario Amin  @file UI/StickyHUD.h
 * @class AStickyHUD
 * @brief Main HUD class for ABaseShooter, derives from AHUD
 * @details Simple HUD class to load the widgets in UI/Widgets/ and set up display
 **/
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
