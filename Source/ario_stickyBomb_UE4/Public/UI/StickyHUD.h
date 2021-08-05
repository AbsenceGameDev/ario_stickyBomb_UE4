// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"
#include "Helpers/Macros.h"

#include <Components/WidgetComponent.h>
#include <GameFramework/HUD.h>

#include "StickyHUD.generated.h"

/**
 * @author  Ario Amin
 * @file    UI/StickyHUD.h
 * @class   AStickyHUD
 * @brief   Main HUD class for ABaseShooter, derives from AHUD
 * @details Simple HUD class to load the widgets in UI/Widgets/ and set up display
 */
UCLASS()
class ARIO_STICKYBOMB_UE4_API AStickyHUD : public AHUD
{
	GENERATED_BODY()

	public:
	/**
	 * @brief   Construct a new AStickyHUD object
	 *
	 */
	AStickyHUD();

	/* ============================ */
	/** Inherited Methods: Overrides */
	protected:
	/**
	 * @brief   Inherited and unused PostInit
	 *
	 */
	virtual void PostInitializeComponents() override;

	/**
	 * @brief   Initialize some components on BeginPlay
	 *
	 */
	virtual void BeginPlay() final;

	public:
	/**
	 * @brief   Inherited and mostly unused - Super::Tick
	 *
	 * @param   DeltaTime
	 */
	virtual void Tick(float DeltaTime) final;

	/**
	 * @brief   Inherited and mostly unused - Super::DrawHUD
	 *
	 */
	virtual void DrawHUD() final;

	/* ================================ */
	/* Public Methods: Client interface */
	/**
	 * @brief   Update Killbox/container with total kills
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "UIActions")
	void UpdateTotalKills();

	/**
	 * @brief   Reflect value to textbox
	 * @details Reflects value from ABaseShooter->StickyGun->AmmoComp->GetAmmoCount(),e
	 */
	UFUNCTION(BlueprintCallable, Category = "UIActions")
	void UpdateAmmo();

	/**
	 * @brief   Toggle (unimplemented) game menu
	 *
	 */
	void ToggleGameMenu();
	// void AppendReloadPrompt();

	/**
	 * @brief   Update widget with Kill (unfinished/untested)
	 *
	 * @param   Kill
	 */
	void AddKillToWidget(FString Kill);

	protected:
	/* =============================== */
	/* Protected Methods: Init Widgets */

	/**
	 * @brief   Init Kill Overlay widget
	 *
	 */
	void InitializeKillOverlayWidget();

	/**
	 * @brief   Init Killbox/container widget
	 *
	 */
	void InitializeTotalKillsWidget();

	/**
	 * @brief   Init AmmoWidget
	 *
	 */
	void InitializeAmmoWidget();

	/* ========================= */
	/* Protected Fields: Widgets */
	TSharedPtr<SKillWidget>						TotalKillsWidget;
	TSharedPtr<SAmmoWidget>						AmmoWidget;
	TSharedPtr<SSlideInText>					OverlayMenu;
	TSharedPtr<SKillContentContainer> KillList;
	TSharedPtr<AStickyHUD>						StickyHUD;

	/* ================================== */
	/* Protected Fields: Basic properties */
	bool bIsTitleVisible;
	bool bIsKillWidgetInitialized;
	bool bIsAmmoWidgetInitialized;
	bool bIsOverlayMenuVisible;
};
