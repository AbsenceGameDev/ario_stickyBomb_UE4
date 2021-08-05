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

/**
 * @author Ario Amin  @file UI/Widgets/SSlideInText.h
 * @class SSlideInText
 * @brief Slide In Text widget, derives from SCompoundWidget
 * @details Simple widget to slide in text upon a player kill.
 */
class ARIO_STICKYBOMB_UE4_API SSlideInText : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SSlideInText) {}
	SLATE_ARGUMENT(FText, TextToShow)
	SLATE_END_ARGS()

	/**
	 * @brief
	 *
	 * @param InArgs
	 */
	void Construct(const FArguments& InArgs);

	/**
	 * @brief
	 *
	 */
	void TransitionIn();

	/**
	 * @brief
	 *
	 */
	void TransitionOut();

	FText TextToShow;

	protected:
	/**
	 * @brief Get the Color object
	 *
	 * @return FLinearColor
	 */
	FLinearColor GetColor() const;

	/**
	 * @brief Get the Item Scale object
	 *
	 * @return FVector2D
	 */
	FVector2D GetItemScale() const;

	EVisibleState	 CurrentState;
	FCurveSequence VisibleAnimation;
	FCurveHandle	 ScaleCurveX;
	FCurveHandle	 ScaleCurveY;
	FCurveSequence FadeAnimation;
	FCurveHandle	 FadeValue;

	private:
	/**
	 * @brief
	 *
	 * @param AllottedGeometry
	 * @param InCurrentTime
	 * @param InDeltaTime
	 */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};
