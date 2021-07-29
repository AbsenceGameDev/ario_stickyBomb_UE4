#pragma once

/**
 * @brief Branchless Numeric If
 **/
#define LL_IF(A, B, CondA) (A * (CondA)) + (B * !(CondA))

/**
 * @brief Branchless Numeric 'If-Else'
 **/
#define LL_IFELSE(A, B, CondA, CondB) (A * (CondA)) + (B * (CondB))

/**
 * @brief Branchless Arithmetic Clamp
 **/
#define LL_CLAMP(Value, Min, Max) (0 * (Value < Min)) + (Max * (Value > Max)) + (Value * (Value > Min && Value < Max))