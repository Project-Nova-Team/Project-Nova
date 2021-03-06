// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectNova : ModuleRules
{
	public ProjectNova(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        if(Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "DetailCustomizations", "PropertyEditor", "EditorStyle" });
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        }

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
