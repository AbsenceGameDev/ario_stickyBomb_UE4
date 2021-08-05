/**
 * @author  Ario Amin
 * @file    UI/Widget/SKillContentContainer.cpp
 * @class   SKillContentContainer
 * @brief   A simple shell of a class to be expanded upon.
 */
#include "UI/Widgets/SKillContentContainer.h"

#include "SlateOptMacros.h"
#include "UI/Widgets/SSlideInText.h"

// Engine Frameworks
#include <Engine/World.h>

// Engine Helpers
#include <Containers/UnrealString.h>
#include <Templates/SharedPointer.h>

// Engine UI
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SWrapBox.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/SListView.h>
#include <Widgets/Views/STableRow.h>

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SKillContentContainer::Construct(const FArguments& InArgs)
{
	OwnerHud		 = InArgs._OwnerHud;
	FontForKills = FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24);
	ChildSlot.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
			[SNew(SWrapBox).Visibility(EVisibility::Visible).PreferredWidth(500.f) +
			 SWrapBox::Slot().Padding(5)[SAssignNew(ListViewWidget, SListView<TSharedPtr<FString>>)
																		 .ItemHeight(24)
																		 .ListItemsSource(&Items)
																		 .OnGenerateRow(this, &SKillContentContainer::OnGenerateRowForList)]];
}

void SKillContentContainer::AddSlot(FString Value)
{
	// @todo Do I need to clear items?
	// Do I need to clear after every round? Performance implications?
	Items.Push(MakeShareable(new FString(Value)));
	ListViewWidget->RequestListRefresh();
}

TSharedRef<ITableRow> SKillContentContainer::OnGenerateRowForList(
	TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		.Padding(2.0f)[SNew(SSlideInText).TextToShow(FText::FromString(*Item.Get()))];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
