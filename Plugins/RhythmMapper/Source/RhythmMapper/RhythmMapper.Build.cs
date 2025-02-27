// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RhythmMapper : ModuleRules
{
	public RhythmMapper(ReadOnlyTargetRules target) : base(target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "UMG" });

		PublicIncludePaths.AddRange(
			[
				// ... add public include paths required here ...
			]
		);
				
		
		PrivateIncludePaths.AddRange(
			[
				// ... add other private include paths required here ...
			]
		);
			
		
		PublicDependencyModuleNames.AddRange(
			[
				"Core"
				// ... add other public dependencies that you statically link with here ...
			]
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"Blutility",
				"UMG",
				"ScriptableEditorWidgets",
				"UMGEditor"
				// ... add private dependencies that you statically link with here ...	
			]
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			[
				// ... add any modules that your module loads dynamically here ...
			]
		);
	}
}
