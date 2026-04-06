// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HW06 : ModuleRules
{
	public HW06(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
