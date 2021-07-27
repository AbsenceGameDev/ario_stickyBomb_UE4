// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/AmmoWidget.h"

#include "Runtime/UMG/Public/UMG.h"

UAmmoWidget::UAmmoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAmmoWidget::NativeConstruct()
{
  Super::NativeConstruct();
}

void UAmmoWidget::InitWidget()
{
	if (AmmoCountTextBlock == nullptr) {
	  AmmoCountTextBlock = NewObject<UTextBlock>();
	}
  AmmoCountTextBlock->SetText(TextResource);
  AmmoCountTextBlock->SetVisibility(ESlateVisibility::Visible);
}

// Assumes previous knowledge of object validity
void UAmmoWidget::HideWidget()
{
  AmmoCountTextBlock->SetVisibility(ESlateVisibility::Hidden);
}

// Assumes previous knowledge of object validity
void UAmmoWidget::ShowWidget()
{
  AmmoCountTextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UAmmoWidget::UpdateAmmoCount(int AmmoCount)
{
	if (AmmoCountTextBlock != nullptr) {
	  TextResource = FText::FromString("AMMO: " + FString::FromInt(AmmoCount));
	  AmmoCountTextBlock->SetText(TextResource);
	}
}

void UAmmoWidget::AppendReloadPrompt()
{
  TextResource = FText::FromString(TextResource.ToString() + " (Reload!)");
  AmmoCountTextBlock->SetText(TextResource);
}
