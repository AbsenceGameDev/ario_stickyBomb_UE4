#pragma once

/**
 * @author Ario Amin  @file Helpers/Macros.h
 * @brief Various macros
 * @details Some arithmetic if statements for efficieny
 * and some shorthands for nasty Slate function calls
 */

#define STICKY_DEBUG

/**
 * @brief Branchless Numeric If
 */
#define LL_IF(A, B, CondA) (A * (CondA)) + (B * !(CondA))

/**
 * @brief Branchless Numeric 'If-Else'
 */
#define LL_IFELSE(A, B, CondA, CondB) (A * (CondA)) + (B * (CondB))

/**
 * @brief Branchless Arithmetic Clamp
 */
#define LL_CLAMP(Value, Min, Max) (0 * (Value < Min)) + (Max * (Value > Max)) + (Value * (Value >= Min && Value <= Max))

/**
 * @brief Print Location
 */
#define DEBUG_PRINT_LOC(Position) \
	UE_LOG(LogTemp, Warning, TEXT("git commit -S -m \"LOCATION: {%f,%f,%f}!\""), Position.X, Position.Y, Position.Z);

#define DEFAULT_STICKY_GUN_MAX 3

/**
 * @brief Regarding Slate SWidget based classes
 * prefix with . when using these during create using SNew()
 */
#define MAKETEXT(InString) Text(FText::FromString(InString))
#define MAKETEXTBOUND(Function) Text(this, &Function)
#define MAKEROBOTO(FontSize) Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize))
#define MAKECOLOR(FLinearColorA, FLinearColorB) ShadowColorAndOpacity(FLinearColorA).ColorAndOpacity(FLinearColorB)

#define MAKESHADOW_OFFSET(OffsetMin, OffsetMax) ShadowOffset(FIntPoint(OffsetMin, OffsetMax))
