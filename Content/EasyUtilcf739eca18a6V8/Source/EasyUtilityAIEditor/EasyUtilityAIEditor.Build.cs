// Easy Utility AI - (C) 2025 by Michael Hegemann

using UnrealBuildTool;

public class EasyUtilityAIEditor : ModuleRules
{
	public EasyUtilityAIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrecompileForTargets = PrecompileTargetsType.Any;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTags",
				"EasyUtilityAI",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"UnrealEd",
			    "AIModule",
				"Slate",
				"SlateCore",
				"Kismet",
				"KismetCompiler",
				"BlueprintGraph",
				"AssetTools",       // class FAssetTypeActions_Base
			});
	}
}
