// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/ForwardDecls.h"

class ARIO_STICKYBOMB_UE4_API SKillContentContainer : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SKillContentContainer)
	{
	}
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
