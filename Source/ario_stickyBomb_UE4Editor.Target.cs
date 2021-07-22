// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ario_stickyBomb_UE4EditorTarget : TargetRules
{
	public ario_stickyBomb_UE4EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("ario_stickyBomb_UE4");
	}
}
