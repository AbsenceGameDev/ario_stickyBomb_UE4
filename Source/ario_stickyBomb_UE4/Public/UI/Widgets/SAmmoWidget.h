
#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SCompoundWidget.h>

/**
 * @author  Ario Amin
 * @file    UI/Widgets/SAmmoWidget.h
 * @class   SAmmoWidget
 * @brief   Ammo Widget which derives from SCompoundWidget
 * @details Simple widget to display the ammo count of the ammo component
 */
class ARIO_STICKYBOMB_UE4_API SAmmoWidget : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SAmmoWidget) : _TextToSet(FText::FromString("")) {}
	SLATE_ARGUMENT(TWeakObjectPtr<AStickyHUD>, OwnerHud);
	SLATE_ARGUMENT(FText, TextToSet)
	SLATE_END_ARGS()

	/**
	 * @brief
	 *
	 * @param InArgs
	 */
	void Construct(const FArguments& InArgs);

	/**
	 * @brief Set the Ammo Text object
	 *
	 * @param Ammo
	 */
	void SetAmmoText(FText Ammo);

	/**
	 * @brief Get the Ammo Text object
	 *
	 * @return FText
	 */
	FText GetAmmoText() const;

	private:
	TWeakObjectPtr<AStickyHUD> OwnerHUD;
	TSharedPtr<STextBlock>		 TextBlockRawPtr;
	FText											 AmmoText;
};
