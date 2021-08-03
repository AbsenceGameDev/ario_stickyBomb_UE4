// Fill out your copyright notice in the Description page of Project Settings.

#include "Effects/MaterialGenerator.h"

#include <AssetRegistry/AssetRegistryModule.h>
#include <Factories/MaterialFactoryNew.h>
#include <Materials/MaterialExpressionAppendVector.h>
// #include <Materials/MaterialExpressionFontSample.h>
#include <MaterialExpressionIO.h>

// Material Expression Types
#include <Materials/MaterialExpressionAdd.h>
#include <Materials/MaterialExpressionCeil.h>
#include <Materials/MaterialExpressionComponentMask.h>
#include <Materials/MaterialExpressionConstant.h>
#include <Materials/MaterialExpressionConstant3Vector.h>
#include <Materials/MaterialExpressionDotProduct.h>
#include <Materials/MaterialExpressionDynamicParameter.h>
#include <Materials/MaterialExpressionLinearInterpolate.h>
#include <Materials/MaterialExpressionMultiply.h>
#include <Materials/MaterialExpressionOneMinus.h>
#include <Materials/MaterialExpressionPanner.h>
#include <Materials/MaterialExpressionScalarParameter.h>
#include <Materials/MaterialExpressionSubtract.h>
#include <Materials/MaterialExpressionTextureCoordinate.h>
#include <Materials/MaterialExpressionTextureSample.h>
#include <Materials/MaterialExpressionVectorParameter.h>
#include <Materials/MaterialExpressionVertexNormalWS.h>

FMaterialGenerator::FMaterialGenerator()
{
	// Do stuff
	CreateBasicMaterial("M_Material", "/Game/GenMaterials/");
	CreateCelShadedExplosionMat("M_Material", "/Game/GenMaterials/");
}

template <class TPackageType>
bool FMaterialGenerator::SaveUPackage(UPackage* Package, TPackageType* UnrealObj, FString& PackageName)
{		 // Saving the uasset so it doesn't have to be saved manually in the editor
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	return UPackage::SavePackage(
		Package, UnrealObj, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true,
		SAVE_NoError);
}

template <class TParamType>
TParamType* FMaterialGenerator::MakeParam(FString ParamName, UMaterial* UnrealMaterial)
{
	TParamType* NewParam = NewObject<TParamType>(UnrealMaterial);
	UnrealMaterial->Expressions.Add(NewParam);
	NewParam->ParameterName = *ParamName;
	// NewParam->DefaultValue	= 1; // turn into input param so it's type can be controlled by extending the template a bit
	return NewParam;
}

template <class TParamType>
TParamType* FMaterialGenerator::MakeArrayParam(TArray<FString> ParamNames, UMaterial* UnrealMaterial)
{
	TParamType* NewParam = NewObject<TParamType>(UnrealMaterial);
	UnrealMaterial->Expressions.Add(NewParam);
	NewParam->ParamNames = *ParamNames;
	// NewParam->DefaultValue	= 1; // turn into input param so it's type can be controlled by extending the template a bit
	return NewParam;
}

template <class TExpressionType>
TExpressionType* FMaterialGenerator::AddExpression(UMaterial* UnrealMaterial)
{
	TExpressionType* Expression = NewObject<TExpressionType>(UnrealMaterial);
	UnrealMaterial->Expressions.Add(Expression);
	return Expression;
}

UMaterialExpressionTextureSample* FMaterialGenerator::MakeTextureSampler(UTexture* DiffuseTexture, UMaterial* UnrealMaterial)
{
	// make texture sampler
	UMaterialExpressionTextureSample* TextureSample = NewObject<UMaterialExpressionTextureSample>(UnrealMaterial);
	TextureSample->Texture													= DiffuseTexture;
	TextureSample->SamplerType											= SAMPLERTYPE_Color;		// make templated Sampler Type
	UnrealMaterial->Expressions.Add(TextureSample);
	UnrealMaterial->BaseColor.Expression = TextureSample;
	return TextureSample;
}

UMaterial* FMaterialGenerator::CreateThenRegisterMaterialObject(UPackage* Package, FString& MaterialBaseName)
{
	auto			 MaterialFactory = NewObject<UMaterialFactoryNew>();
	UMaterial* UnrealMaterial	 = (UMaterial*) MaterialFactory->FactoryCreateNew(
		 UMaterial::StaticClass(), Package, *MaterialBaseName, RF_Standalone | RF_Public, NULL, GWarn);
	FAssetRegistryModule::AssetCreated(UnrealMaterial);
	return UnrealMaterial;
}

FMaterialGenerator* FMaterialGenerator::CreateObject() { return new FMaterialGenerator; }

void FMaterialGenerator::CreateBasicMaterial(FString MaterialBaseName, FString PackageName)
{
	PackageName += MaterialBaseName;
	// if (FindPackage(*PackageName) == nullptr) {
	// 	return;		 // Return if package already exists, make a seperate function for editing materials
	// }
	UPackage* Package = CreatePackage(*PackageName);

	// create an unreal material asset
	UMaterial* UnrealMaterial = CreateThenRegisterMaterialObject(Package, PackageName);
	Package->FullyLoad();
	Package->SetDirtyFlag(true);

	// Tiling system
	auto Multiply = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);

	// Diffuse
	// /Engine/EngineResources/WhiteSquareTexture
	// /Engine/MaterialTemplates/Textures/T_Noise01
	FStringAssetReference DiffuseAssetPath("/Engine/MaterialTemplates/Textures/T_Noise01");
	UTexture*							DiffuseTexture = Cast<UTexture>(DiffuseAssetPath.TryLoad());
	if (DiffuseTexture) {
		// make texture sampler
		UMaterialExpressionTextureSample* TextureExpression = MakeTextureSampler(DiffuseTexture, UnrealMaterial);

		// Tiling
		TextureExpression->Coordinates.Expression = Multiply;
	}

	// Tiling
	auto Append						 = AddExpression<UMaterialExpressionAppendVector>(UnrealMaterial);
	Multiply->B.Expression = Append;

	auto TexCoords				 = AddExpression<UMaterialExpressionTextureCoordinate>(UnrealMaterial);
	Multiply->A.Expression = TexCoords;

	auto XParam					 = MakeParam<UMaterialExpressionScalarParameter>(FString("TextureRepeatX"), UnrealMaterial);
	Append->A.Expression = XParam;

	// Make Y Parameter and hook it into AppendB
	auto YParam					 = MakeParam<UMaterialExpressionScalarParameter>(FString("TextureRepeatY"), UnrealMaterial);
	Append->B.Expression = YParam;

	// Hook multiply into base
	UnrealMaterial->BaseColor.Expression = Multiply;

	// let the material update itself if necessary
	UnrealMaterial->PreEditChange(NULL);
	UnrealMaterial->PostEditChange();

	// make sure that any static meshes, etc using this material will stop using the FMaterialResource of the original
	// material, and will use the new FMaterialResource created when we make a new UMaterial in place
	FGlobalComponentReregisterContext RecreateComponents;

	// SaveMaterial(Package, UnrealMaterial, PackageName);
	SaveUPackage<UMaterial>(Package, UnrealMaterial, PackageName);
}

void FMaterialGenerator::CreateCelShadedExplosionMat(FString MaterialBaseName, FString PackageName)
{
	PackageName += MaterialBaseName;
	// if (FindPackage(*PackageName) == nullptr) {
	// 	return;		 // Return if package already exists, make a seperate function for editing materials
	// }
	UPackage* Package = CreatePackage(*PackageName);

	// create an unreal material asset
	UMaterial* UnrealMaterial = CreateThenRegisterMaterialObject(Package, PackageName);
	Package->FullyLoad();
	Package->SetDirtyFlag(true);

	// Diffuse
	// /Engine/EngineResources/WhiteSquareTexture
	// /Engine/MaterialTemplates/Textures/T_Noise01
	FStringAssetReference DiffuseAssetPath("/Engine/MaterialTemplates/Textures/T_Noise01");
	UTexture*							DiffuseTexture = Cast<UTexture>(DiffuseAssetPath.TryLoad());
	if (DiffuseTexture) {
		// Light Direction
		auto ComponentMask_LD	 = AddExpression<UMaterialExpressionComponentMask>(UnrealMaterial);
		auto DotP_LD					 = AddExpression<UMaterialExpressionDotProduct>(UnrealMaterial);
		auto VertexNormalWS_LD = AddExpression<UMaterialExpressionVertexNormalWS>(UnrealMaterial);

		// UV Setup
		// UV LerpA,              Hook: Vector3 -> LerpA,
		auto UVVector3 = AddExpression<UMaterialExpressionConstant3Vector>(UnrealMaterial);
		// UV LerpB (Distortion), Hook: TexCoord -> Panner -> TextureSample1 -> LeprB,
		auto TexCoord_UV					 = AddExpression<UMaterialExpressionTextureCoordinate>(UnrealMaterial);
		auto Panner_UV						 = AddExpression<UMaterialExpressionPanner>(UnrealMaterial);
		auto TextureExpression_UV1 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMaterialExpressionTextureSample*
		// UV LerpAlpha,          Hook: TextureSample2 -> LerpAlpha
		auto TextureExpression_UV2 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMaterialExpressionTextureSample*

		// UV Lerp Hook
		auto Lerp_UV = AddExpression<UMaterialExpressionLinearInterpolate>(UnrealMaterial);

		// TextureSample_GreenInjection
		auto TextureExpression_Green = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*

		// Small highlights,
		// Hook: TexExpr -> Muliply1_B -> Multiply2_A -> Multiply3_A -> OUT
		// Hook: GreenInjection_RGB -> Muliply1_A -> ... -> OUT
		// Hook: GreenVectorParam -> Muliply2_B -> ... -> OUT
		// Hook: GreenScalarParam -> Muliply3_B -> ... -> OUT
		auto TextureExpression_SmallHighlights = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample
		auto Multiply_SmallHighlights1				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_SmallHighlights2				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_SmallHighlights3				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto GreenScalarParam									 = MakeParam<UMaterialExpressionScalarParameter>(FString("G_Scalar"), UnrealMaterial);
		auto GreenVectorParam									 = MakeParam<UMaterialExpressionVectorParameter>(FString("G_Vector"), UnrealMaterial);

		// Dissolve_Emissive
		// Hook: TexExpr -> Subtract1_A -> Multiply1_A -> Saturate -> Multiply2_A -> Ceil -> Multiply3_B -> Multiply4_A -> OUT
		// Hook: DynParam_emissive_dissolve -> 1-x -> Subtract1_B -> ... -> OUT
		// Hook: DissolveScalarParam -> Multiply1_B -> ... -> OUT
		// Hook: ConstantExpression -> Multiply2_B
		// Hook: DynParam_emissive_fade -> Multiply3_B
		auto DynamicDissolveParam = MakeParam<UMaterialExpressionDynamicParameter>(FString("Dyn_Dissolve"), UnrealMaterial);
		auto TextureExpression_Dissolve_Gradient = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto Multiply_Dissolve_Emmisive1				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emmisive2				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emmisive3				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emmisive4				 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);

		auto OneMinus_Dissolve_Emmissive = AddExpression<UMaterialExpressionOneMinus>(UnrealMaterial);
		auto Subtract_Dissolve_Emmissive = AddExpression<UMaterialExpressionSubtract>(UnrealMaterial);
		auto Ceil_Dissolve_Emmissive		 = AddExpression<UMaterialExpressionCeil>(UnrealMaterial);
		auto Constant_Dissolve_Emmissive = AddExpression<UMaterialExpressionConstant>(UnrealMaterial);

		// VerticalDissolve
		// Hook: TextureObject -> TexObject_T2D__WorldAlignedTexture__XYTex -> Add_B -> OUT
		// Hook: ScalarParam_Size -> TexSize_V3__WorldAlignedTexture__XYTex -> ... -> OUT
		// Hook: DynParam_dissolve_vertical -> Add_A -> OUT
		// UTexture2D* T2D_VerticalDissolve = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone |
		// RF_MarkAsRootSet);
		auto TextureExpression_VerticalDissolve = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		 // UMatExprTextureSample*
		auto Add_VerticalDissolve								= AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto ScalarParam_VerticalDissolve = MakeParam<UMaterialExpressionScalarParameter>(FString("VertDissolve"), UnrealMaterial);

		// Dissolve
		// Hook: TexExpr1 -> Mask(R,G) -> Multiply1_A -> Add1_B -> TexExpr2_UVs (R) -> Lerp1_A -> OUT
		// Hook: TexCoord -> UVs_V2_UVsScaleUVsByCenter -> Add1_A -> ... -> OUT
		// Hook: ScalarParam_Scale -> TexSCale_V2_UVsScaleUVsByCenter -> ... -> OUT
		// Hook: ConstantExpr -> Lerp1_B -> OUT
		// Hook: DynParam_dissolve -> 1-x -> LerpAlpha
		auto TextureExpression_Dissolve	 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto MaskRG_Dissolve						 = AddExpression<UMaterialExpressionComponentMask>(UnrealMaterial);
		auto Multiply_Dissolve					 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Add_Dissolve								 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto TextureExpression_Dissolve2 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto Lerp_Dissolve							 = AddExpression<UMaterialExpressionLinearInterpolate>(UnrealMaterial);
		auto ScalarParam_Dissolve				 = MakeParam<UMaterialExpressionScalarParameter>(FString("Dissolve"), UnrealMaterial);
		auto ConstanExpr_Dissolve				 = AddExpression<UMaterialExpressionConstant>(UnrealMaterial);
		auto OneMinus_Dissolve					 = AddExpression<UMaterialExpressionOneMinus>(UnrealMaterial);

		// Dissolve + VerticalDissolve  (OpacityMask)
		// Hook: VerticalDissolve_OUT -> Subtract_A -> OUT
		// Hook: Dissolve_OUT -> Subtract_B -> OpacityMask
		auto Subtract_OpacityMask = AddExpression<UMaterialExpressionSubtract>(UnrealMaterial);

		// Everything else  (Emmisive)
		// ============================================================ Vector3Param -> Multiply1_A -> ...
		// Hook: LightDirection_OUT -> DotUV_A -> TextureSample_GreenInjection -> Add1_A -> Multiply1_B -> Add2_A -> Emmisive(OUT)
		// Hook: UV_OUT             -> DotUV_B
		auto DotUV_Emissive					 = AddExpression<UMaterialExpressionDotProduct>(UnrealMaterial);
		auto Add_OpacityMask1				 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto Add_OpacityMask2				 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto Multiply_OpacityMask		 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto VectorParam_OpacityMask = MakeParam<UMaterialExpressionVectorParameter>(FString("OpacityMask_Vector"), UnrealMaterial);

		// DotUV -> TextureSample_GreenInjection
		// DotUV -> TextureSample(Small Highlights/ WhiteBarBottom)
		// DotUV -> TextureSample(Dissolve_Emissive)

		// Hook into Basecolor
		UnrealMaterial->BaseColor.Expression = ComponentMask_LD;
	}

	// let the material update itself if necessary
	UnrealMaterial->PreEditChange(NULL);
	UnrealMaterial->PostEditChange();

	// make sure that any static meshes, etc using this material will stop using the FMaterialResource of the original
	// material, and will use the new FMaterialResource created when we make a new UMaterial in place
	FGlobalComponentReregisterContext RecreateComponents;

	// SaveMaterial(Package, UnrealMaterial, PackageName);
	SaveUPackage<UMaterial>(Package, UnrealMaterial, PackageName);
}