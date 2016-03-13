
namespace UnrealBuildTool.Rules
{
    public class MpoCA : ModuleRules
    {
        public MpoCA(TargetInfo Target)
        {
            //PublicIncludePaths.AddRange(new string[] {});

            //PrivateIncludePaths.AddRange(new string[] { });


            /** Names of modules with header files that this module's public interface needs access to. */
            //PublicIncludePathModuleNames.AddRange(new string[] {  });

            /** Names of modules with header files that this module's private implementation needs access to. */
            //PrivateIncludePathModuleNames.AddRange(new string[] { });


            /** Names of modules that this module's public interface depends on. */
            PublicDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent"  });

            /** Names of modules that this module's private implementation depends on. */
            PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });


            /** Extra modules this module may require at run time */
            //DynamicallyLoadedModuleNames.AddRange(new string[] { "" });
        }
    }
}