// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class ARIO_STICKYBOMB_UE4_API FMaterialGenerator
{
	FMaterialGenerator();

	template <class TPackageType>
	bool SaveUPackage(UPackage* Package, TPackageType* UnrealObj, FString& PackageName);

	template <class TParamType>
	TParamType* MakeParam(FString ParamName, UMaterial* UnrealMaterial);

	template <class TParamType>
	TParamType* FMaterialGenerator::MakeArrayParam(TArray<FString> ParamNames, UMaterial* UnrealMaterial);

	template <class TExpressionType>
	TExpressionType* AddExpression(UMaterial* UnrealMaterial);

	UMaterialExpressionTextureSample* MakeTextureSampler(UTexture* DiffuseTexture, UMaterial* UnrealMaterial);
	UMaterial*												CreateThenRegisterMaterialObject(UPackage* Package, FString& MaterialBaseName);

	public:
	static FMaterialGenerator* CreateObject();
	void											 CreateBasicMaterial(FString MaterialBaseName = "M_Material", FString PackageName = "/Game/");
	void CreateCelShadedExplosionMat(FString MaterialBaseName = "M_CelMaterial", FString PackageName = "/Game/");
};
