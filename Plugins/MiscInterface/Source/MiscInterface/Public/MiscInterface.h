// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMiscInterface, Log, All);

class FMiscInterfaceModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterSentryCrashDiagnostics();
	void UnregisterSentryCrashDiagnostics();
	void InitLuaCrashLogPath();
	void AppendLuaCrashInfo(const TCHAR* PlatformName, int32 Signal);

	FDelegateHandle SentryCrashInfoInitHandle;
	FDelegateHandle SentryCrashInfoAppendHandle;
	FString LuaCallStackLogPath;
};
