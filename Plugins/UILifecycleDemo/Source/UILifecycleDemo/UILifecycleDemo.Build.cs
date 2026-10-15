using UnrealBuildTool;

public class UILifecycleDemo : ModuleRules
{
	public UILifecycleDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG"
			}
		);
	}
}
