// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commandlets/ExportAssetT3DCommandlet.h"

#if WITH_EDITOR

#include "Exporters/Exporter.h"
#include "AssetExportTask.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/GCObjectScopeGuard.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#endif // WITH_EDITOR

DEFINE_LOG_CATEGORY_STATIC(LogExportAssetT3D, Log, All);

UExportAssetT3DCommandlet::UExportAssetT3DCommandlet(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UExportAssetT3DCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR

    TArray<FString> Tokens;
    TArray<FString> Switches;
    TMap<FString, FString> ParamVals;
    ParseCommandLine(*Params, Tokens, Switches, ParamVals);

    const FString AssetPath    = ParamVals.FindRef(TEXT("asset"));
    const FString AssetListFile = ParamVals.FindRef(TEXT("assetlist"));
    const FString ExportPath   = ParamVals.FindRef(TEXT("exportpath"));

    // 参数校验
    if (AssetPath.IsEmpty() && AssetListFile.IsEmpty())
    {
        UE_LOG(LogExportAssetT3D, Error, TEXT("Missing required parameter. Use -asset=\"/Game/...\" or -assetlist=\"path/to/list.txt\""));
        return 1;
    }
    if (ExportPath.IsEmpty())
    {
        UE_LOG(LogExportAssetT3D, Error, TEXT("Missing required parameter: -exportpath=\"D:/OutputDir/\""));
        return 1;
    }

    // 收集待导出的资产路径
    TArray<FString> AssetPaths;
    if (!AssetPath.IsEmpty())
    {
        AssetPaths.Add(AssetPath);
    }
    else
    {
        FString FileContent;
        if (!FFileHelper::LoadFileToString(FileContent, *AssetListFile))
        {
            UE_LOG(LogExportAssetT3D, Error, TEXT("Cannot read asset list file: %s"), *AssetListFile);
            return 1;
        }
        FileContent.ParseIntoArrayLines(AssetPaths, true);
        UE_LOG(LogExportAssetT3D, Display, TEXT("Loaded %d asset paths from: %s"), AssetPaths.Num(), *AssetListFile);
    }

    int32 SuccessCount = 0;
    int32 FailCount    = 0;

    for (const FString& CurrentAssetPath : AssetPaths)
    {
        if (CurrentAssetPath.IsEmpty())
        {
            continue;
        }

        UObject* Asset = LoadObject<UObject>(nullptr, *CurrentAssetPath);
        if (!Asset)
        {
            UE_LOG(LogExportAssetT3D, Error, TEXT("Failed to load asset: %s"), *CurrentAssetPath);
            ++FailCount;
            continue;
        }

        // 直接以 ExportPath 为输出目录，文件名取资产名
        const FString OutputFile = FPaths::Combine(ExportPath, Asset->GetName()) + TEXT(".t3d");

        if (!IFileManager::Get().MakeDirectory(*ExportPath, true))
        {
            UE_LOG(LogExportAssetT3D, Error, TEXT("Failed to create output directory: %s"), *ExportPath);
            ++FailCount;
            continue;
        }

        UAssetExportTask* ExportTask = NewObject<UAssetExportTask>();
        FGCObjectScopeGuard ExportTaskGuard(ExportTask);
        ExportTask->Object            = Asset;
        ExportTask->Filename          = OutputFile;
        ExportTask->bSelected         = false;
        ExportTask->bReplaceIdentical = true;
        ExportTask->bPrompt           = false;
        ExportTask->bUseFileArchive   = false;
        ExportTask->bWriteEmptyFiles  = false;

        if (UExporter::RunAssetExportTask(ExportTask))
        {
            UE_LOG(LogExportAssetT3D, Display, TEXT("[OK] %s -> %s"), *CurrentAssetPath, *OutputFile);
            ++SuccessCount;
        }
        else
        {
            UE_LOG(LogExportAssetT3D, Error, TEXT("[FAIL] No exporter found for: %s"), *CurrentAssetPath);
            ++FailCount;
        }
    }

    UE_LOG(LogExportAssetT3D, Display, TEXT("========================================"));
    UE_LOG(LogExportAssetT3D, Display, TEXT("Export complete. Success: %d  Failed: %d"), SuccessCount, FailCount);
    UE_LOG(LogExportAssetT3D, Display, TEXT("========================================"));

    return (FailCount > 0) ? 1 : 0;

#else
    UE_LOG(LogExportAssetT3D, Error, TEXT("ExportAssetT3D commandlet requires an editor build."));
    return 1;
#endif // WITH_EDITOR
}
