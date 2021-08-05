// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

#include <Widgets/SCompoundWidget.h>

/**
 * @author Ario Amin  @file UI/SInGameOverlay.h
 * @class SInGameOverlay
 * @brief Game Overlay widget, derives from SCompoundWidget
 * @details Simple widget to use a in game overlay.
 */
class ARIO_STICKYBOMB_UE4_API SInGameOverlay : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SInGameOverlay) {}
	SLATE_END_ARGS()

	/**
	 * @brief
	 *
	 * @param InArgs
	 */
	void Construct(const FArguments& InArgs);
};
