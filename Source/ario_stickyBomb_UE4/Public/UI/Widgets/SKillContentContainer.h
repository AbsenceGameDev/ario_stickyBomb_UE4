// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Containers/UnrealString.h>
#include <Engine/World.h>
#include <Templates/SharedPointer.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/SListView.h>
#include <Widgets/Views/STableRow.h>

class SSlideInText;
class AStickyHUD;

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
  AStickyHUD* OwnerHud;

  TArray<TSharedPtr<FString>> Items;

  TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> NewItem, const TSharedRef<STableViewBase>& OwnerTable);

  TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;

  FSlateFontInfo FontForKills;
};
