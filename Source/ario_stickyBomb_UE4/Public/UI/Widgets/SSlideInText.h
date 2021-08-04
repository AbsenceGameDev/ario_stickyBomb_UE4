// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

enum class EVisibleState : uint8 {
	VS_Animating_To_Show,
	VS_Animating_To_Hide,
	VS_Visible,
	VS_Hidden,
};

class ARIO_STICKYBOMB_UE4_API SSlideInText : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SSlideInText) {}
	SLATE_ARGUMENT(FText, TextToShow)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void TransitionIn();
	void TransitionOut();

	FText TextToShow;

	protected:
	FLinearColor GetColor() const;
	FVector2D		 GetItemScale() const;

	EVisibleState	 CurrentState;
	FCurveSequence VisibleAnimation;
	FCurveHandle	 ScaleCurveX;
	FCurveHandle	 ScaleCurveY;
	FCurveSequence FadeAnimation;
	FCurveHandle	 FadeValue;

	private:
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};
