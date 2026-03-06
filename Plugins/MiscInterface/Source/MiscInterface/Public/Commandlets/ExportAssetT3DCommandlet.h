// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "ExportAssetT3DCommandlet.generated.h"

/**
 * Commandlet for exporting UE assets to T3D format without launching the editor UI.
 *
 * Usage:
 *   UnrealEditor.exe [Project.uproject] -run=ExportAssetT3D -asset="/Game/Path/To/Asset" -exportpath="D:/Output/"
 *   UnrealEditor.exe [Project.uproject] -run=ExportAssetT3D -assetlist="D:/list.txt" -exportpath="D:/Output/"
 *
 * Parameters:
 *   -asset=      Single UE asset path (e.g. /Game/Characters/MyChar)
 *   -assetlist=  Path to a text file containing one asset path per line
 *   -exportpath= Output directory on disk
 */
UCLASS()
class MISCINTERFACE_API UExportAssetT3DCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UExportAssetT3DCommandlet(const FObjectInitializer& ObjectInitializer = FObjectInitializer());

    virtual int32 Main(const FString& Params) override;
};
