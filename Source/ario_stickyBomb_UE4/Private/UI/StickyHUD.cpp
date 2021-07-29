// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StickyHUD.h"

#include "StickyGameMode.h"
#include "StickyPlayerState.h"
#include "UI/Widgets/SKillContentContainer.h"
#include "UI/Widgets/SKillWidget.h"
#include "UI/Widgets/SSlideInText.h"

#include <Engine/Engine.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SWeakWidget.h>

AStickyHUD::AStickyHUD()
{
	// AmmoCountWidgetClass = UAmmoWidget::StaticClass();
	// HealthWidgetClass = UHealthWidget::StaticClass();

	bIsTitleVisible					 = false;
	bisKillWidgetInitialized = false;
	bisOverlayMenuVisible		 = false;
}

/** ============================ **/
/** Inherited Methods: Overrides **/

void AStickyHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AStickyHUD::BeginPlay()
{
	Super::BeginPlay();

	// if (AmmoWidget == nullptr) {
	//   AmmoWidget = NewObject<UAmmoWidget>();
	// }
	// if (HealthWidget == nullptr) {
	//   HealthWidget = NewObject<UHealthWidget>();
	// }
	// AmmoWidget->InitWidget();

	InitializeTotalKillsWidget();
	InitializeKillOverlayWidget();
}

void AStickyHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AStickyHUD::DrawHUD()
{
	Super::DrawHUD();
}

/** ================================ **/
/** Public Methods: Client interface **/

void AStickyHUD::UpdateTotalKills()
{
	if (!bisKillWidgetInitialized) return;

	APlayerController* OwningPlayerController = this->GetOwningPlayerController();
	if (OwningPlayerController == nullptr) return;

	AStickyPlayerState* PlayerState	 = Cast<AStickyPlayerState>(OwningPlayerController->PlayerState);
	int32								UpdatedKills = PlayerState != nullptr ? PlayerState->GetKills() : -1;
	if (UpdatedKills == -1) return;

	TotalKillsWidget->SetVisibility(EVisibility::Visible);
	FText ScoreUpdate = FText::Format(NSLOCTEXT("GameFlow", "KillNr", "Kills {0}"), FText::AsNumber(UpdatedKills));
	TotalKillsWidget->SetScoreText(ScoreUpdate);
}

// void AStickyHUD::UpdateAmmoWidget(int AmmoCount)
// {
// 	if (AmmoWidget != nullptr) {
// 	  AmmoWidget->UpdateAmmoCount(AmmoCount);
// 	}
// }

void AStickyHUD::ToggleGameMenu()
{
	// @todo this will be the game menu to hold all in game inventory, char, weapons
}

// void AStickyHUD::AppendReloadPrompt()
// {
// 	if (AmmoWidget != nullptr) {
// 	  AmmoWidget->AppendReloadPrompt();
// 	}
// }

void AStickyHUD::AddKillToWidget(FString Kill)
{
	/* Should be for example: "Erik Killed Enemy #1 */
	KillList->AddSlot(Kill);
}

/** =============================== **/
/** Protected Methods: Init Widgets **/
void AStickyHUD::InitializeTotalKillsWidget()
{
	if (bisKillWidgetInitialized) return;

	APlayerController*	OwningPlayerController = this->GetOwningPlayerController();
	AStickyPlayerState* PlayerState =
		OwningPlayerController != nullptr ? Cast<AStickyPlayerState>(OwningPlayerController->PlayerState) : nullptr;
	int32 PlayerKills = PlayerState != nullptr ? PlayerState->GetKills() : 0;

	FText ScoreUpdate = FText::Format(NSLOCTEXT("GameFlow", "KillNr", "Kills {0}"), FText::AsNumber(PlayerKills));
	TotalKillsWidget	= SNew(SKillWidget).OwnerHud(this).TextToSet(ScoreUpdate);
	GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(TotalKillsWidget.ToSharedRef()));
	bisKillWidgetInitialized = true;

	if (PlayerKills == 0) TotalKillsWidget->SetVisibility(EVisibility::Hidden);
}

void AStickyHUD::InitializeKillOverlayWidget()
{
	KillList = SNew(SKillContentContainer).OwnerHud(this);
	if (KillList != nullptr) {
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(KillList.ToSharedRef()));
	}
}
