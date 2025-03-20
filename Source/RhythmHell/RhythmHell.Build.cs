// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RhythmHell : ModuleRules
{
	public RhythmHell(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Json", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EditorScriptingUtilities" });
	}
}