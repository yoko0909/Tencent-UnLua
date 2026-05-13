#include "ReferenceFinderTestLibrary.h"

#include "Kismet/KismetSystemLibrary.h"
#include "UObject/UObjectGlobals.h"

namespace ReferenceFinderTest
{
    static FString DescribeObject(const UObject* Object)
    {
        if (!Object)
        {
            return TEXT("None");
        }

        const UObject* Outer = Object->GetOuter();
        return FString::Printf(
            TEXT("%s | Class=%s | Outer=%s"),
            *Object->GetPathName(),
            *Object->GetClass()->GetPathName(),
            Outer ? *Outer->GetPathName() : TEXT("None")
        );
    }

    static void PrintLine(UObject* WorldContextObject, const FString& Line, bool bPrintToScreen, bool bPrintToLog)
    {
        if (bPrintToLog)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Line);
        }

        if (bPrintToScreen)
        {
            UKismetSystemLibrary::PrintString(
                WorldContextObject,
                Line,
                true,
                false,
                FLinearColor(0.0f, 0.66f, 1.0f),
                10.0f
            );
        }
    }
}

UObject* UReferenceFinderTestLibrary::GetClassDefaultObjectForTest(UClass* Class)
{
    return Class ? Class->GetDefaultObject() : nullptr;
}

void UReferenceFinderTestLibrary::RunReferenceFinder(
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
)
{
    OutReferences.Reset();
    OutReferencePaths.Reset();

    if (!TargetObject)
    {
        ReferenceFinderTest::PrintLine(
            WorldContextObject,
            TEXT("[ReferenceFinder] TargetObject is null."),
            bPrintToScreen,
            bPrintToLog
        );
        return;
    }

    FReferenceFinder Finder(
        OutReferences,
        LimitOuter,
        bRequireDirectOuter,
        bShouldIgnoreArchetype,
        bSerializeRecursively,
        bShouldIgnoreTransient
    );

    Finder.FindReferences(TargetObject);

    for (UObject* Reference : OutReferences)
    {
        OutReferencePaths.Add(ReferenceFinderTest::DescribeObject(Reference));
    }

    ReferenceFinderTest::PrintLine(
        WorldContextObject,
        FString::Printf(
            TEXT("[ReferenceFinder] Target=%s"),
            *ReferenceFinderTest::DescribeObject(TargetObject)
        ),
        bPrintToScreen,
        bPrintToLog
    );

    ReferenceFinderTest::PrintLine(
        WorldContextObject,
        FString::Printf(
            TEXT("[ReferenceFinder] LimitOuter=%s RequireDirectOuter=%s IgnoreArchetype=%s Recursive=%s IgnoreTransient=%s Count=%d"),
            *ReferenceFinderTest::DescribeObject(LimitOuter),
            bRequireDirectOuter ? TEXT("true") : TEXT("false"),
            bShouldIgnoreArchetype ? TEXT("true") : TEXT("false"),
            bSerializeRecursively ? TEXT("true") : TEXT("false"),
            bShouldIgnoreTransient ? TEXT("true") : TEXT("false"),
            OutReferences.Num()
        ),
        bPrintToScreen,
        bPrintToLog
    );

    for (int32 Index = 0; Index < OutReferencePaths.Num(); ++Index)
    {
        ReferenceFinderTest::PrintLine(
            WorldContextObject,
            FString::Printf(TEXT("[ReferenceFinder][%d] %s"), Index, *OutReferencePaths[Index]),
            bPrintToScreen,
            bPrintToLog
        );
    }
}
