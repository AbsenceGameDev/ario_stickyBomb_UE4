// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

#include <Widgets/SCompoundWidget.h>

/**
 *
 */
class ARIO_STICKYBOMB_UE4_API SInGameOverlay : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SInGameOverlay) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
