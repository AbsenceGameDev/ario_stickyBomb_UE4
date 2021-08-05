// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

/**
 * @author Ario Amin  @file UI/Widgets/SKillContentContainer.h
 * @class SKillContentContainer
 * @brief Kill Container widget, derives from SCompoundWidget
 * @details Simple widget to display the kills in a cornered box.
 **/
class ARIO_STICKYBOMB_UE4_API SKillContentContainer : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SKillContentContainer) {}
	SLATE_ARGUMENT(class AStickyHUD*, OwnerHud)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void AddSlot(FString Value);

	protected:
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> NewItem, const TSharedRef<STableViewBase>& OwnerTable);

	AStickyHUD*																 OwnerHud;
	TArray<TSharedPtr<FString>>								 Items;
	TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;
	FSlateFontInfo														 FontForKills;
};
