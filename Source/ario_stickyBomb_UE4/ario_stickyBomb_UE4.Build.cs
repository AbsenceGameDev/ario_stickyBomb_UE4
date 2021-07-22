// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ario_stickyBomb_UE4 : ModuleRules
{
	public ario_stickyBomb_UE4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
