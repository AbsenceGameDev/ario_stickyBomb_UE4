// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

#include "FPlayerData.generated.h"

/**
 * @author Ario Amin  @file UI/FPlayerData.h
 * @struct FPlayerData
 * @brief Simple utility struct to pass data into widgets.
 **/
USTRUCT()
struct ARIO_STICKYBOMB_UE4_API FPlayerData {
	GENERATED_BODY()

	float		Score;
	FString Name;

	FPlayerData(float score = 0.0f, FString name = FString("No Player Name")) : Score(score), Name(name) {}
};
