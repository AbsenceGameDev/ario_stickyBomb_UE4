// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

/**
 * @author Ario Amin  @file Effects/MaterialGenerator.h
 * @class FMaterialGenerator
 * @brief A WIP utility class for Material creation/editing
 * @details This class lets you create new packages, design them through code and save
 * them to your content_browser (Currently only Materials).
 * End result is a uasset primed for used in the editor, it can therefor be used to create templates.
 *
 * The Actual idea with this class was to be able to create an explosion material during BeginPlaym
 * which would have been used in the actual explosion effects of the StickyProjectile.
 *
 * @todo Extend with many variants of templates, both in regards to expression types but also package types,
 * and opt for selective compilation with 'constexpr if's for efficiency.
 */
class ARIO_STICKYBOMB_UE4_API FMaterialGenerator
{
	/**
	 * @brief Construct a new FMaterialGenerator object
	 * @note This CTOR is private and note exposed, use ::CreateObject() to generate one
	 */
	FMaterialGenerator();

	/**
	 * @brief
	 *
	 * @tparam TPackageType
	 * @param Package
	 * @param UnrealObj
	 * @param PackageName
	 * @return true
	 * @return false
	 */
	template <class TPackageType>
	bool SaveUPackage(UPackage* Package, TPackageType* UnrealObj, FString& PackageName);

	/**
	 * @brief
	 *
	 * @tparam TParamType
	 * @param ParamName
	 * @param UnrealMaterial
	 * @return TParamType*
	 */
	template <class TParamType>
	TParamType* MakeParam(FString ParamName, UMaterial* UnrealMaterial);

	/**
	 * @brief
	 *
	 * @tparam TParamType
	 * @param ParamNames
	 * @param UnrealMaterial
	 * @return TParamType*
	 */
	template <class TParamType>
	TParamType* MakeArrayParam(TArray<FString> ParamNames, UMaterial* UnrealMaterial);

	/**
	 * @brief
	 *
	 * @tparam TExpressionType
	 * @param UnrealMaterial
	 * @return TExpressionType*
	 */
	template <class TExpressionType>
	TExpressionType* AddExpression(UMaterial* UnrealMaterial);

	/**
	 * @brief
	 *
	 * @param DiffuseTexture
	 * @param UnrealMaterial
	 * @return UMaterialExpressionTextureSample*
	 */
	UMaterialExpressionTextureSample* MakeTextureSampler(UTexture* DiffuseTexture, UMaterial* UnrealMaterial);

	/**
	 * @brief Create a Then Register Material Object object
	 *
	 * @param Package
	 * @param MaterialBaseName
	 * @return UMaterial*
	 */
	UMaterial* CreateThenRegisterMaterialObject(UPackage* Package, FString& MaterialBaseName);

	public:
	/**
	 * @brief Create a FMaterialGenerator object
	 *
	 * @return FMaterialGenerator*
	 */
	static FMaterialGenerator* CreateObject();

	/**
	 * @brief Create a Basic Material object
	 *
	 * @param MaterialBaseName
	 * @param PackageName
	 */
	void CreateBasicMaterial(FString MaterialBaseName = "M_Material", FString PackageName = "/Game/GenMaterials/");

	/**
	 * @brief Create a Cel Shaded Explosion Mat object
	 *
	 * @param MaterialBaseName
	 * @param PackageName
	 */
	void CreateCelShadedExplosionMat(
		FString MaterialBaseName = FString("M_CelExplosionMat"), FString PackageName = FString("/Game/GenMaterials/"));
};
