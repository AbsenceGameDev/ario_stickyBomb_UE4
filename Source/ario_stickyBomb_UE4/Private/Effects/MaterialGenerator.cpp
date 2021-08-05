/**
 * @author  Ario Amin
 * @file    Effects/MaterialGenerator.cpp
 * @class   AStickyBaseActor
 * @brief   A simple utility class for generating and editing materialgraphs in code.
 * @todo    Pehaps the class needs to be extended to be be able to edit other editor uassets
 **/
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
#include <Materials/MaterialExpressionSaturate.h>
#include <Materials/MaterialExpressionScalarParameter.h>
#include <Materials/MaterialExpressionSubtract.h>
#include <Materials/MaterialExpressionTextureCoordinate.h>
#include <Materials/MaterialExpressionTextureSample.h>
#include <Materials/MaterialExpressionVectorParameter.h>
#include <Materials/MaterialExpressionVertexNormalWS.h>

FMaterialGenerator::FMaterialGenerator()
{
	// Do stuff
	// CreateBasicMaterial("M_Material", "/Game/GenMaterials/");
	// CreateCelShadedExplosionMat("M_Material", "/Game/GenMaterials/");
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
	NewParam->ParamNames = ParamNames;
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
	// SaveUPackage<UMaterial>(Package, UnrealMaterial, PackageName);
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(
		Package, UnrealMaterial, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true,
		SAVE_NoError);
}

void FMaterialGenerator::CreateCelShadedExplosionMat(FString MaterialBaseName, FString PackageName)
{
	PackageName += MaterialBaseName;
	// if (FindPackage(*PackageName) == nullptr) {
	// 	return;		 // Return if package already exists, make a seperate function for editing materials
	// }
	UPackage* Package = CreatePackage(*PackageName);

	// create an unreal material asset
	UMaterial* UnrealMaterial = CreateThenRegisterMaterialObject(Package, MaterialBaseName);
	Package->FullyLoad();
	Package->SetDirtyFlag(true);

	// Diffuse
	// /Engine/EngineResources/WhiteSquareTexture
	// /Engine/MaterialTemplates/Textures/T_Noise01
	FStringAssetReference DiffuseAssetPath("/Engine/MaterialTemplates/Textures/T_Noise01");
	UTexture*							DiffuseTexture = Cast<UTexture>(DiffuseAssetPath.TryLoad());
	if (DiffuseTexture) {
		// Light Direction,
		// Hook: VectorParam  -> Mask(RGB) -> DotA
		// Hook: VertexNormal -> DotB
		auto VectorParam_LD		 = MakeParam<UMaterialExpressionVectorParameter>(FString("LightDirection"), UnrealMaterial);
		auto ComponentMask_LD	 = AddExpression<UMaterialExpressionComponentMask>(UnrealMaterial);
		auto DotP_LD					 = AddExpression<UMaterialExpressionDotProduct>(UnrealMaterial);
		auto VertexNormalWS_LD = AddExpression<UMaterialExpressionVertexNormalWS>(UnrealMaterial);

		ComponentMask_LD->Input.Expression = VectorParam_LD;			 // Hook Vector Param into Mask
		DotP_LD->A.Expression							 = ComponentMask_LD;		 // Hook Mask into Dot
		DotP_LD->B.Expression							 = VertexNormalWS_LD;		 // Hook VetexNormalWS into Dot

		// UV Setup
		// UV LerpA,              Hook: Vector3 -> LerpA,
		// UV LerpB (Distortion), Hook: TexCoord -> Panner -> TextureSample1 -> LeprB,
		// UV LerpAlpha,          Hook: TextureSample2 -> LerpAlpha
		auto UVVector3						 = AddExpression<UMaterialExpressionConstant3Vector>(UnrealMaterial);
		auto TexCoord_UV					 = AddExpression<UMaterialExpressionTextureCoordinate>(UnrealMaterial);
		auto Panner_UV						 = AddExpression<UMaterialExpressionPanner>(UnrealMaterial);
		auto TextureExpression_UV1 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMaterialExpressionTextureSample*
		auto TextureExpression_UV2 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMaterialExpressionTextureSample*
		auto Lerp_UV							 = AddExpression<UMaterialExpressionLinearInterpolate>(UnrealMaterial);

		Lerp_UV->A.Expression													= UVVector3;								// Hook Vector3 Constant into Lerp A
		Panner_UV->Coordinate.Expression							= TexCoord_UV;							// Hook TextureCoords into Panner
		TextureExpression_UV1->Coordinates.Expression = Panner_UV;								// Hook Panner into TextureParam_UV
		Lerp_UV->B.Expression													= TextureExpression_UV1;		// Hook TextureParam Constant into Lerp B
		Lerp_UV->Alpha.Expression											= TextureExpression_UV2;		// Hook TextureParam Constant into Lerp Alpha

		auto DotUV_Emissive					 = AddExpression<UMaterialExpressionDotProduct>(UnrealMaterial);
		DotUV_Emissive->A.Expression = DotP_LD;
		DotUV_Emissive->B.Expression = Lerp_UV;
		// TextureSample_GreenInjection
		auto TextureExpression_GreenInjection		 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto TextureExpression_SmallHighlights	 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample
		auto TextureExpression_Dissolve_Gradient = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*

		TextureExpression_GreenInjection->Coordinates.Expression		= DotUV_Emissive;		 // Hook DotUV_Emissive as UV Coords
		TextureExpression_SmallHighlights->Coordinates.Expression		= DotUV_Emissive;		 // Hook DotUV_Emissive as UV Coords
		TextureExpression_Dissolve_Gradient->Coordinates.Expression = DotUV_Emissive;		 // Hook DotUV_Emissive as UV Coords

		// Small highlights,
		auto Multiply_SmallHighlights1	 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_SmallHighlights2	 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_SmallHighlights3	 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto ScalarParam_SmallHighlights = MakeParam<UMaterialExpressionScalarParameter>(FString("G_Scalar"), UnrealMaterial);
		auto VectorParam_SmallHighlights = MakeParam<UMaterialExpressionVectorParameter>(FString("G_Vector"), UnrealMaterial);

		// Hook graph ndoes together
		// Hook: TexExpr -> Muliply1_B -> Multiply2_A -> Multiply3_A -> OUT
		Multiply_SmallHighlights1->B.Expression = TextureExpression_SmallHighlights;
		Multiply_SmallHighlights2->A.Expression = Multiply_SmallHighlights1;
		Multiply_SmallHighlights3->A.Expression = Multiply_SmallHighlights2;

		// Hook: GreenInjection -> Muliply1_A -> ... -> OUT
		Multiply_SmallHighlights1->A.Expression = TextureExpression_GreenInjection;

		// Hook: GreenVectorParam -> Muliply2_B -> ... -> OUT
		Multiply_SmallHighlights2->B.Expression = VectorParam_SmallHighlights;

		// Hook: GreenScalarParam -> Muliply3_B -> ... -> OUT
		Multiply_SmallHighlights3->B.Expression = ScalarParam_SmallHighlights;

		////
		//// Dissolve_Emissive
		TArray<FString> DynamicFields;
		DynamicFields.Add(TEXT("dissolve_vertical"));
		DynamicFields.Add(TEXT("dissolve"));
		DynamicFields.Add(TEXT("emissive_fade"));
		DynamicFields.Add(TEXT("emissive_dissolve"));
		// auto DynamicDissolveParam				 = MakeArrayParam<UMaterialExpressionDynamicParameter>(DynamicFields, UnrealMaterial);
		auto Multiply_Dissolve_Emissive1 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emissive2 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emissive3 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Multiply_Dissolve_Emissive4 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);

		auto ScalarParam_Dissolve_Emissive = MakeParam<UMaterialExpressionScalarParameter>(FString("Dissolve_Scalar"), UnrealMaterial);
		auto OneMinus_Dissolve_Emissive		 = AddExpression<UMaterialExpressionOneMinus>(UnrealMaterial);
		auto Subtract_Dissolve_Emissive		 = AddExpression<UMaterialExpressionSubtract>(UnrealMaterial);
		auto Ceil_Dissolve_Emissive				 = AddExpression<UMaterialExpressionCeil>(UnrealMaterial);
		auto Saturate_Dissolve_Emissive		 = AddExpression<UMaterialExpressionSaturate>(UnrealMaterial);
		auto Constant_Dissolve_Emissive		 = AddExpression<UMaterialExpressionConstant>(UnrealMaterial);

		// Hook: TexExpr -> Subtract1_A -> Multiply1_A -> Saturate -> Multiply2_A -> Ceil -> Multiply3_B -> Multiply4_A -> OUT
		Subtract_Dissolve_Emissive->A.Expression		 = TextureExpression_Dissolve_Gradient;
		Multiply_Dissolve_Emissive1->A.Expression		 = Subtract_Dissolve_Emissive;
		Saturate_Dissolve_Emissive->Input.Expression = Multiply_Dissolve_Emissive1;
		Multiply_Dissolve_Emissive2->A.Expression		 = Saturate_Dissolve_Emissive;
		Ceil_Dissolve_Emissive->Input.Expression		 = Multiply_Dissolve_Emissive2;
		Multiply_Dissolve_Emissive3->B.Expression		 = Ceil_Dissolve_Emissive;
		Multiply_Dissolve_Emissive4->A.Expression		 = Multiply_Dissolve_Emissive3;

		// Hook: DynParam_emissive_dissolve -> 1-x -> Subtract1_B -> ... -> OUT
		// OneMinus_Dissolve_Emissive->Input.Expression = DynamicDissolveParam;
		Subtract_Dissolve_Emissive->B.Expression = OneMinus_Dissolve_Emissive;

		// Hook: DissolveScalarParam -> Multiply1_B -> ... -> OUT
		Multiply_Dissolve_Emissive1->B.Expression = ScalarParam_Dissolve_Emissive;

		// Hook: ConstantExpression -> Multiply2_B -> ... -> OUT
		Multiply_Dissolve_Emissive2->B.Expression = Constant_Dissolve_Emissive;

		// Hook: DynParam_emissive_fade -> Multiply3_B -> ... -> OUT
		// Multiply_Dissolve_Emissive3->B.Expression = DynamicDissolveParam;

		// VerticalDissolve
		// Hook: TextureObject -> TexObject_T2D__WorldAlignedTexture__XYTex -> Add_B -> OUT
		// Hook: ScalarParam_Size -> TexSize_V3__WorldAlignedTexture__XYTex -> ... -> OUT
		// Hook: DynParam_dissolve_vertical -> Add_A -> OUT
		// UTexture2D* T2D_VerticalDissolve = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone |
		// RF_MarkAsRootSet);
		auto TextureExpression_VerticalDissolve = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		 // UMatExprTextureSample*
		auto Add_VerticalDissolve								= AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto ScalarParam_VerticalDissolve = MakeParam<UMaterialExpressionScalarParameter>(FString("VertDissolve"), UnrealMaterial);

		// WorldAlignedTexture->TexObj.Expression = TexObject_T2D;
		// Add_VerticalDissolve->A.Expression     = DynamicDissolveParam; // dissolve vertical
		// Add_VerticalDissolve->B.Expression     = WorldAlignedTexture;

		////
		//// Dissolve
		auto TextureExpression_Dissolve	 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto MaskRG_Dissolve						 = AddExpression<UMaterialExpressionComponentMask>(UnrealMaterial);
		auto Multiply_Dissolve					 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto Add_Dissolve								 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto TextureExpression_Dissolve2 = MakeTextureSampler(DiffuseTexture, UnrealMaterial);		// UMatExprTextureSample*
		auto Lerp_Dissolve							 = AddExpression<UMaterialExpressionLinearInterpolate>(UnrealMaterial);
		auto ScalarParam_Dissolve				 = MakeParam<UMaterialExpressionScalarParameter>(FString("Dissolve"), UnrealMaterial);
		auto OneMinus_Dissolve					 = AddExpression<UMaterialExpressionOneMinus>(UnrealMaterial);
		auto TexCoord_Dissolve					 = AddExpression<UMaterialExpressionTextureCoordinate>(UnrealMaterial);

		// Hook: TexExpr1 -> Mask(R,G) -> Multiply1_A -> Add1_B -> TexExpr2_UVs (R) -> Lerp1_A -> OUT
		MaskRG_Dissolve->Input.Expression										= TextureExpression_Dissolve;
		Multiply_Dissolve->A.Expression											= MaskRG_Dissolve;
		Add_Dissolve->B.Expression													= Multiply_Dissolve;
		TextureExpression_Dissolve2->Coordinates.Expression = Add_Dissolve;
		Lerp_Dissolve->A.Expression													= TextureExpression_Dissolve2;		// R Channel

		// Hook: TexCoord -> UVs_V2_UVsScaleUVsByCenter -> Add1_A -> ... -> OUT
		// Hook: ScalarParam_Scale -> TexSCale_V2_UVsScaleUVsByCenter -> ... -> OUT

		// Hook: ConstantExpr -> Lerp1_B -> OUT
		Lerp_Dissolve->ConstB = 1.0f;

		// Hook: DynParam_dissolve -> 1-x -> LerpAlpha
		// OneMinus_Dissolve->Input.Expression = DynamicDissolveParam;		 // dissolve param
		Lerp_Dissolve->Alpha.Expression = OneMinus_Dissolve;

		// Dissolve + VerticalDissolve  (OpacityMask)
		// Hook: VerticalDissolve_OUT -> Subtract_A -> OUT
		// Hook: Dissolve_OUT -> Subtract_B -> OpacityMask
		auto Subtract_OpacityMask = AddExpression<UMaterialExpressionSubtract>(UnrealMaterial);

		////
		//// Everything else  (Emissive)
		// ================================================================ Vector3Param -> Multiply1_A -> ...
		// Hook: LightDirection_OUT -> DotUV_A -> TextureSample_GreenInjection -> Add1_A -> Multiply1_B -> Add2_A -> Emissive(OUT)
		// Hook: SmallHighlights_OUT -> Add1_B -> ...
		// Hook: UV_OUT             -> DotUV_B
		auto Add_OpacityMask1				 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto Add_OpacityMask2				 = AddExpression<UMaterialExpressionAdd>(UnrealMaterial);
		auto Multiply_OpacityMask		 = AddExpression<UMaterialExpressionMultiply>(UnrealMaterial);
		auto VectorParam_OpacityMask = MakeParam<UMaterialExpressionVectorParameter>(FString("OpacityMask_Vector"), UnrealMaterial);

		Add_OpacityMask1->A.Expression		 = TextureExpression_GreenInjection;
		Add_OpacityMask1->B.Expression		 = Multiply_SmallHighlights3;
		Multiply_OpacityMask->A.Expression = VectorParam_OpacityMask;
		Multiply_OpacityMask->B.Expression = Add_OpacityMask1;
		Add_OpacityMask2->A.Expression		 = Multiply_OpacityMask;

		// Hook into EmissiveColor
		UnrealMaterial->EmissiveColor.Expression = Add_OpacityMask2;

		// Hook into OpacityMask
		Subtract_OpacityMask->A.Expression		 = Add_VerticalDissolve;
		Subtract_OpacityMask->B.Expression		 = Lerp_Dissolve;
		UnrealMaterial->OpacityMask.Expression = Subtract_OpacityMask;
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
