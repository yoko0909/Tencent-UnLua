#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReferenceFinderTestLibrary.generated.h"

UCLASS()
class TPSPROJECT_API UReferenceFinderTestLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Reference Finder Test")
    static UObject* GetClassDefaultObjectForTest(UClass* Class);

    UFUNCTION(BlueprintCallable, Category = "Reference Finder Test", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "WorldContextObject,bPrintToScreen,bPrintToLog"))
    static void RunReferenceFinder(
        UObject* WorldContextObject,
        UObject* TargetObject,
        UObject* LimitOuter,
        bool bRequireDirectOuter,
        bool bShouldIgnoreArchetype,
        bool bSerializeRecursively,
        bool bShouldIgnoreTransient,
        bool bPrintToScreen,
        bool bPrintToLog,
        TArray<UObject*>& OutReferences,
        TArray<FString>& OutReferencePaths
    );
};
