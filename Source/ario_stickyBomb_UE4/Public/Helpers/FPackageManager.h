// Ario Amin - 2021/08

#pragma once

#include "CoreMinimal.h"

#include <AssetRegistry/AssetRegistryModule.h>
#include <Factories/MaterialFactoryNew.h>
#include <Factories/MaterialParameterCollectionFactoryNew.h>
#include <Materials/MaterialParameterCollection.h>

/**
 * @brief SAVE UPACKAGE MACRO
 *
 * @param Package         -- Type UPackage
 * @param UnrealObj       -- Type Object
 * @param PackageFileName -- Type FString
 */
#define SAVE_UPACKAGE(Package, UnrealObj, PackageFileName)                                                                    \
	UPackage::SavePackage(                                                                                                      \
		Package, UnrealObj, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, \
		SAVE_NoError);

/**
 * @brief FACTORY CREATE NEW MACRO
 *
 * @param UClass      -- The class which we are working with
 * @param Package     -- Type UPackage
 * @param BaseName    -- Type FString
 */
#define MAKE_FACTORY_NEW(UClass, Package, BaseName) \
	FactoryCreateNew(UClass::StaticClass(), Package, *BaseName, RF_Standalone | RF_Public, NULL, GWarn);

/**
 * @brief MAKE PACKAGE FILENAME MACRO
 *
 * @param PackageName    -- Type FString
 */
#define MAKE_PACKAGE_FILENAME(PackageName) \
	FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

/**
 * @brief PREPARE PACKAGE FOR EDIT MACRO
 *
 * @param PackageName    -- Type FString
 */
#define PREP_EDIT_PACKAGE(Package, UnrealType)    \
	FAssetRegistryModule::AssetCreated(UnrealType); \
	Package->FullyLoad();                           \
	Package->SetDirtyFlag(true);

/**
 * @author  Ario Amin
 * @file    Helpers/FPackageManager.h
 * @class   FPackageManager
 * @brief   A WIP utility class for UAsset creation/saving
 * @details This class lets you create new packages and save them to your content_browser.
 *          End result is a uasset primed for used in the editor, it can therefor be used to create templates.
 *
 */
class ARIO_STICKYBOMB_UE4_API FPackageManager
{
	/**
	 * @brief   Construct a new FPackageManager object
	 * @details This CTOR is private, use ::CreatePackageManager() to generate one
	 */
	FPackageManager();

	public:
	/**
	 * @brief Construct UPackage of requested type
	 *
	 * @tparam TPackageTypeFactoryNew
	 * @tparam TPackageType
	 * @param PackageName
	 * @param BaseName
	 * @return UPackage*
	 */
	template <class TPackageTypeFactoryNew, class TPackageType>
	UPackage* ConstructUPackage(FString PackageName, FString BaseName, TPackageType*& UnrealType);

	/**
	 * @brief   Save UPackage of requested type
	 *
	 * @tparam  TPackageType
	 * @param   Package
	 * @param   UnrealObj
	 * @param   PackageName
	 * @return  true | false
	 */
	template <class TPackageType>
	bool SaveUPackage(UPackage* Package, TPackageType* UnrealObj, FString& PackageName);

	/**
	 * @brief   Create a FPackageManager object
	 *
	 * @return  FPackageManager*
	 */
	static FPackageManager* CreateObject();

	/**
	 * @brief   Delete a FPackageManager object
	 *
	 * @param   PackMan - PackageManager object to delete
	 */
	static void DeleteObject(FPackageManager* PackMan);
};

template <class TPackageTypeFactoryNew, class TPackageType>
UPackage* FPackageManager::ConstructUPackage(FString PackageName, FString BaseName, TPackageType*& UnrealType)
{
	UPackage* Package			= CreatePackage(*PackageName);
	auto			TypeFactory = NewObject<TPackageTypeFactoryNew>();

	UnrealType = (TPackageType*) TypeFactory->MAKE_FACTORY_NEW(TPackageType, Package, BaseName);
	PREP_EDIT_PACKAGE(Package, UnrealType);

	return Package;
}

template <class TPackageType>
bool FPackageManager::SaveUPackage(UPackage* Package, TPackageType* UnrealObj, FString& PackageName)
{
	// let the uobject update itself if necessary
	UnrealObj->PreEditChange(NULL);
	UnrealObj->PostEditChange();

	FGlobalComponentReregisterContext RecreateComponents;		 // Make sure actors using this UAsset refresh resources to update.

	FString PackageFileName = MAKE_PACKAGE_FILENAME(PackageName);
	return SAVE_UPACKAGE(Package, UnrealObj, PackageFileName);
}
