// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/SKillWidget.h"

#include "SlateOptMacros.h"

#include <Internationalization/Internationalization.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Text/STextBlock.h>

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SScoreWidget"

void SKillWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD	= InArgs._OwnerHud;
	ScoreText = InArgs._TextToSet;

	FSlateFontInfo ResultFont = FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24);

	ChildSlot.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
			[SNew(SOverlay) + SOverlay::Slot()
													.VAlign(VAlign_Center)
													.HAlign(HAlign_Center)[SNew(STextBlock)
																									 .ShadowColorAndOpacity(FLinearColor::Black)
																									 .ColorAndOpacity(FLinearColor::Green)
																									 .ShadowOffset(FIntPoint(-1, 1))
																									 .Font(ResultFont)
																									 .Text(this, &SKillWidget::GetScoreText) /* Polling for score updates */
	]];
}

void SKillWidget::SetScoreText(FText Score)
{
	ScoreText = Score;
}

FText SKillWidget::GetScoreText() const
{
	return ScoreText;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
