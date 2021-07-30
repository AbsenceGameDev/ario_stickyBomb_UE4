// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/SAmmoWidget.h"

#include "Helpers/Macros.h"
#include "SlateOptMacros.h"

#include <Internationalization/Internationalization.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Text/STextBlock.h>

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SAmmoWidget"

void SAmmoWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHud;
	AmmoText = InArgs._TextToSet;

	TSharedRef<STextBlock> LocalTextBlock = SNew(STextBlock)
																						.MAKECOLOR(FLinearColor::Black, FLinearColor::Green)
																						.MAKESHADOW_OFFSET(-1, 1)
																						.MAKEROBOTO(68)
																						.MAKETEXTBOUND(SAmmoWidget::GetAmmoText);
	ChildSlot.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Left)[SNew(SOverlay) + SOverlay::Slot().VAlign(VAlign_Center).HAlign(HAlign_Center)[LocalTextBlock]];
}

void	SAmmoWidget::SetAmmoText(FText Ammo) { AmmoText = Ammo; }
FText SAmmoWidget::GetAmmoText() const { return AmmoText; }

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
