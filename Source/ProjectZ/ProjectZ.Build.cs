// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ProjectZ : ModuleRules
{
	public ProjectZ(ReadOnlyTargetRules Target) : base(Target)
	{
        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //PrivatePCHHeaderFile = "ProjectZ.h";

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Landscape", "Niagara", "Water", "UMG", "GameplayTasks", "AIModule", "NavigationSystem", "GameplayCameras", "ProceduralMeshComponent" });
        
        PublicIncludePaths.AddRange( new string[] { "ProjectZ/" } );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
