/**
 * @author  Ario Amin
 * @file    UI/Widgets/SSlideInText.cpp
 * @class   SSlideInText
 * @brief   A simple widget that kan be used to display a recent player kill.
 */
#include "UI/Widgets/SSlideInText.h"

#include "SlateOptMacros.h"

// Engine Frameworks
#include <Animation/CurveHandle.h>
#include <Animation/CurveSequence.h>
#include <Engine/World.h>

// Engine Helpers
#include <Engine/EngineTypes.h>
#include <Fonts/SlateFontInfo.h>
#include <Misc/Paths.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

// Engine UI
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SOverlay.h>

/*

	Take a look at FCurveSequence and FCurveHandle

*/

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSlideInText::Construct(const FArguments& InArgs)
{
	TextToShow	 = InArgs._TextToShow;
	CurrentState = EVisibleState::VS_Hidden;
	this->SetVisibility(EVisibility::Collapsed);
	FSlateFontInfo ResultFont = FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24);

	VisibleAnimation = FCurveSequence();
	ScaleCurveX			 = VisibleAnimation.AddCurve(1.0f, 1.0f, ECurveEaseFunction::QuadIn);
	ScaleCurveY			 = VisibleAnimation.AddCurve(1.5f, 1.0f, ECurveEaseFunction::QuadIn);

	FadeAnimation = FCurveSequence();
	FadeValue			= FadeAnimation.AddCurve(0.0f, 5.0, ECurveEaseFunction::QuadIn);

	ChildSlot.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)[SNew(SBorder)
														 .DesiredSizeScale(this, &SSlideInText::GetItemScale)
														 .ColorAndOpacity(this, &SSlideInText::GetColor)[SNew(STextBlock).Font(ResultFont).Text(TextToShow)]];

	this->TransitionIn();
	FadeAnimation.JumpToEnd();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSlideInText::TransitionIn()
{
	if (CurrentState == EVisibleState::VS_Visible) {
		return;
	}
	this->SetVisibility(EVisibility::Visible);
	VisibleAnimation.Play(this->AsShared());
	CurrentState = EVisibleState::VS_Animating_To_Show;
}

void SSlideInText::TransitionOut()
{
	if (CurrentState == EVisibleState::VS_Hidden) {
		return;
	}
	CurrentState = EVisibleState::VS_Animating_To_Hide;
	FadeAnimation.PlayReverse(this->AsShared());
}

FVector2D SSlideInText::GetItemScale() const { return FVector2D(ScaleCurveX.GetLerp(), ScaleCurveY.GetLerp()); }

FLinearColor SSlideInText::GetColor() const
{
	return FMath::Lerp(FLinearColor(1, 1, 1, 0), FLinearColor(1, 1, 1, 1), FadeValue.GetLerp());
}

void SSlideInText::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// animating to show ->
	if (VisibleAnimation.IsAtEnd() && CurrentState == EVisibleState::VS_Animating_To_Show) {
		CurrentState = EVisibleState::VS_Visible;
		TransitionOut();
	}

	// animating to hide ->
	if (FadeAnimation.IsAtStart() && CurrentState == EVisibleState::VS_Animating_To_Hide) {
		CurrentState = EVisibleState::VS_Hidden;
		this->SetVisibility(EVisibility::Hidden);
	}
}