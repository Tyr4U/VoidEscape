// Easy Utility AI - (C) 2025 by Michael Hegemann

using UnrealBuildTool;

public class EasyUtilityAI : ModuleRules
{
	public EasyUtilityAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrecompileForTargets = PrecompileTargetsType.Any;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				"GameplayTags",
				"NavigationSystem",
			}
			);
				
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "AIModule",
			}
			);
	}
}
