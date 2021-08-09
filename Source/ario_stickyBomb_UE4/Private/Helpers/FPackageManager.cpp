/**
 * @author  Ario Amin
 * @file    Effects/MaterialGenerator.cpp
 * @class   AStickyBaseActor
 * @brief   A simple utility class for generating and editing materialgraphs in code.
 * @todo    Pehaps the class needs to be extended to be be able to edit other editor uassets
 */
#include "Helpers/FPackageManager.h"

FPackageManager::FPackageManager()
{
	// Do stuff
}

FPackageManager* FPackageManager::CreateObject() { return new FPackageManager; }

void FPackageManager::DeleteObject(FPackageManager* PackMan) { delete PackMan; }