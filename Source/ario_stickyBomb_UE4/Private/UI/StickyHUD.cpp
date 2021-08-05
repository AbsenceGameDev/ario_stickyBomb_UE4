/**
 * @author  Ario Amin
 * @file    UI/StickyHUD.cpp
 * @class   AStickyHUD
 * @brief   A simple HUD class to handle widgets.
 */
#include "UI/StickyHUD.h"

// Frameworks
#include "StickyGameMode.h"
#include "StickyPlayerState.h"

// Helpers
#include "Helpers/Macros.h"

// UI
#include "UI/Widgets/SAmmoWidget.h"
#include "UI/Widgets/SKillContentContainer.h"
#include "UI/Widgets/SKillWidget.h"
#include "UI/Widgets/SSlideInText.h"

// Engine Classes/Types
#include <Engine/Engine.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SWeakWidget.h>

AStickyHUD::AStickyHUD()
{
	// AmmoCountWidgetClass = UAmmoWidget::StaticClass();
	// HealthWidgetClass = UHealthWidget::StaticClass();

	bIsTitleVisible					 = false;
	bIsKillWidgetInitialized = false;
	bIsAmmoWidgetInitialized = false;
	bIsOverlayMenuVisible		 = false;
}

/* ============================ */
/** Inherited Methods: Overrides */

void AStickyHUD::PostInitializeComponents() { Super::PostInitializeComponents(); }

void AStickyHUD::BeginPlay()
{
	Super::BeginPlay();
	InitializeTotalKillsWidget();
	InitializeAmmoWidget();
	InitializeKillOverlayWidget();
}

void AStickyHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AStickyHUD::DrawHUD() { Super::DrawHUD(); }

/* ================================ */
/* Public Methods: Client interface */

void AStickyHUD::UpdateTotalKills()
{
	if (!bIsKillWidgetInitialized) {
		return;
	}
	APlayerController* OwningPlayerController = this->GetOwningPlayerController();
	if (OwningPlayerController == nullptr) {
		return;
	}

	AStickyPlayerState* PlayerState = Cast<AStickyPlayerState>(OwningPlayerController->PlayerState);
	if (PlayerState != nullptr) {
		return;
	}
	int32 UpdatedKills = PlayerState->GetKills();

	TotalKillsWidget->SetVisibility(EVisibility::Visible);
	FText ScoreUpdate = FText::FromString(FString("Kills: ") + FString::FromInt(UpdatedKills));
	TotalKillsWidget->SetScoreText(ScoreUpdate);
}

void AStickyHUD::UpdateAmmo()
{
	if (!bIsAmmoWidgetInitialized) {
		return;
	}

	APlayerController* OwningPlayerController = this->GetOwningPlayerController();
	if (OwningPlayerController == nullptr) {
		return;
	}

	AStickyPlayerState* PlayerState = Cast<AStickyPlayerState>(OwningPlayerController->PlayerState);
	if (PlayerState == nullptr) {
		return;
	}

	int32 UpdatedAmmo = PlayerState->GetAmmo();

	AmmoWidget->SetVisibility(EVisibility::Visible);
	FText AmmoUpdate = FText::AsNumber(UpdatedAmmo);
	AmmoWidget->SetAmmoText(AmmoUpdate);
}

void AStickyHUD::ToggleGameMenu()
{
	// @todo this will be the game menu to hold all in game inventory, char, weapons
}

void AStickyHUD::AddKillToWidget(FString Kill)
{
	/* Should be for example: "Player_X Killed Player_Y */
	KillList->AddSlot(Kill);
}

/* =============================== */
/* Protected Methods: Init Widgets */
void AStickyHUD::InitializeTotalKillsWidget()
{
	if (bIsKillWidgetInitialized) {
		return;
	}

	APlayerController*	OwningPlayerController = this->GetOwningPlayerController();
	AStickyPlayerState* PlayerState =
		OwningPlayerController != nullptr ? Cast<AStickyPlayerState>(OwningPlayerController->PlayerState) : nullptr;

	int32 PlayerKills = PlayerState != nullptr ? PlayerState->GetKills() : 0;
	FText ScoreUpdate = FText::Format(NSLOCTEXT("GameFlow", "KillNr", "Kills {0}"), FText::AsNumber(PlayerKills));
	TotalKillsWidget	= SNew(SKillWidget).OwnerHud(this).TextToSet(ScoreUpdate);

	GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(TotalKillsWidget.ToSharedRef()));
	TotalKillsWidget->SetVisibility(EVisibility::Visible);
	bIsKillWidgetInitialized = true;
}

void AStickyHUD::InitializeKillOverlayWidget()
{
	KillList = SNew(SKillContentContainer).OwnerHud(this);
	if (KillList != nullptr) {
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(KillList.ToSharedRef()));
	}
}

void AStickyHUD::InitializeAmmoWidget()
{
	if (bIsAmmoWidgetInitialized) {
		return;
	}

	APlayerController* OwningPlayerController = this->GetOwningPlayerController();

	AStickyPlayerState* PlayerState =
		OwningPlayerController != nullptr ? Cast<AStickyPlayerState>(OwningPlayerController->PlayerState) : nullptr;

	int32 PlayerAmmo = PlayerState != nullptr ? PlayerState->GetAmmo() : 0;

	// Will have to rewrite this, just too tired to find the bug right now
	FText AmmoUpdate = FText::AsNumber(DEFAULT_STICKY_GUN_MAX);
	AmmoWidget			 = SNew(SAmmoWidget).OwnerHud(this).TextToSet(AmmoUpdate);

	GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(AmmoWidget.ToSharedRef()));
	AmmoWidget->SetVisibility(EVisibility::Visible);
	bIsAmmoWidgetInitialized = true;
}