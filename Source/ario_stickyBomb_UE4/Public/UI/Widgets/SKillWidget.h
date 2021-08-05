
#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SCompoundWidget.h>

/**
 * @author  Ario Amin
 * @file    UI/Widgets/SKillWidget.h
 * @class   SKillWidget
 * @brief   Kill Count widget, derives from SCompoundWidget
 * @details Simple widget to display the kill count.
 */
class ARIO_STICKYBOMB_UE4_API SKillWidget : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SKillWidget) : _TextToSet(FText::FromString("")) {}
	SLATE_ARGUMENT(TWeakObjectPtr<class AStickyHUD>, OwnerHud);
	SLATE_ARGUMENT(FText, TextToSet)
	SLATE_END_ARGS()

	/**
	 * @brief   Construct Widget
	 *
	 * @param   InArgs
	 */
	void Construct(const FArguments& InArgs);

	/**
	 * @brief   Set the Score Text
	 *
	 * @param   Score
	 */
	void SetScoreText(FText Score);

	/**
	 * @brief   Get the Score Text
	 *
	 * @return  FText
	 */
	FText GetScoreText() const;

	private:
	TWeakObjectPtr<class AStickyHUD> OwnerHUD;
	FText														 ScoreText;
};
