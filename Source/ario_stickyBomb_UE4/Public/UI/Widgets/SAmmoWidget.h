
#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SCompoundWidget.h>

class ARIO_STICKYBOMB_UE4_API SAmmoWidget : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SAmmoWidget) : _TextToSet(FText::FromString("")) {}
	SLATE_ARGUMENT(TWeakObjectPtr<AStickyHUD>, OwnerHud);
	SLATE_ARGUMENT(FText, TextToSet)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	SetAmmoText(FText Ammo);
	FText GetAmmoText() const;

	private:
	TWeakObjectPtr<AStickyHUD> OwnerHUD;
	TSharedPtr<STextBlock>		 TextBlockRawPtr;
	FText											 AmmoText;
};
