// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ario_stickyBomb_UE4 : ModuleRules
{
	public ario_stickyBomb_UE4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});
    PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore"});
    PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    
    PublicDependencyModuleNames.AddRange(new string[] {"UnrealEd"});
    PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "Engine" });
	}
}
