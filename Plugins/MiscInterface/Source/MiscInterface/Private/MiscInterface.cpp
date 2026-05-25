// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscInterface.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "SentryCrashLogAppender.h"
#include "UnLuaDebugBase.h"
#include "UnLuaModule.h"

#include <errno.h>
#include <stdio.h>

DEFINE_LOG_CATEGORY(LogMiscInterface);

#define LOCTEXT_NAMESPACE "FMiscInterfaceModule"

void FMiscInterfaceModule::StartupModule()
{
	RegisterSentryCrashDiagnostics();
}

void FMiscInterfaceModule::ShutdownModule()
{
	UnregisterSentryCrashDiagnostics();
}

void FMiscInterfaceModule::RegisterSentryCrashDiagnostics()
{
	if (!SentryCrashInfoInitHandle.IsValid())
	{
		SentryCrashInfoInitHandle = FSentryCrashLogAppender::RegisterCrashInfoInitializer(
			FSentryCrashInfoInitDelegate::FDelegate::CreateRaw(this, &FMiscInterfaceModule::InitLuaCrashLogPath));
	}

	if (!SentryCrashInfoAppendHandle.IsValid())
	{
		SentryCrashInfoAppendHandle = FSentryCrashLogAppender::RegisterCrashInfoAppender(
			FSentryCrashInfoAppendDelegate::FDelegate::CreateRaw(this, &FMiscInterfaceModule::AppendLuaCrashInfo));
	}
}

void FMiscInterfaceModule::UnregisterSentryCrashDiagnostics()
{
	FSentryCrashLogAppender::UnregisterCrashInfoAppender(SentryCrashInfoAppendHandle);
	FSentryCrashLogAppender::UnregisterCrashInfoInitializer(SentryCrashInfoInitHandle);

	SentryCrashInfoAppendHandle.Reset();
	SentryCrashInfoInitHandle.Reset();
}

void FMiscInterfaceModule::InitLuaCrashLogPath()
{
	const FString LuaCallStackRelativePath = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("LuaCallStack.log"));
	LuaCallStackLogPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*LuaCallStackRelativePath);

	FILE* FilePtr = fopen(TCHAR_TO_UTF8(*LuaCallStackLogPath), "ab+");
	if (FilePtr != nullptr)
	{
		fclose(FilePtr);
		UE_LOG(LogMiscInterface, Log, TEXT("[Yoko.Guo] Lua crash stack log path initialized: %s"), *LuaCallStackLogPath);
	}
	else
	{
		const int32 OpenError = errno;
		UE_LOG(LogMiscInterface, Warning, TEXT("[Yoko.Guo] Failed to initialize Lua crash stack log path: %s errno=%d"), *LuaCallStackLogPath, OpenError);
	}
}

// Diagnostic bridge: this may run from Sentry's crash-time hook. Calling UnLua and
// writing files here is not async-signal-safe, so this path is for validation only.
void FMiscInterfaceModule::AppendLuaCrashInfo(const TCHAR* PlatformName, int32 Signal)
{
	if (LuaCallStackLogPath.IsEmpty())
	{
		InitLuaCrashLogPath();
	}

	UnLua::FLuaEnv* EnvPtr = IUnLuaModule::Get().GetEnv();
	if (EnvPtr == nullptr)
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[Yoko.Guo] MiscInterface Lua crash stack skipped: no UnLua env Platform=%s Signal=%d"), PlatformName, Signal);
		return;
	}

	lua_State* L = EnvPtr->GetMainState();
	if (L == nullptr)
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[Yoko.Guo] MiscInterface Lua crash stack skipped: no Lua state Platform=%s Signal=%d"), PlatformName, Signal);
		return;
	}

	const FString LuaCallStack = UnLua::GetLuaCallStack(L);
	if (LuaCallStack.Len() == 0)
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[Yoko.Guo] MiscInterface Lua crash stack skipped: empty stack Platform=%s Signal=%d"), PlatformName, Signal);
		return;
	}

	FILE* FilePtr = fopen(TCHAR_TO_UTF8(*LuaCallStackLogPath), "ab+");
	if (FilePtr == nullptr)
	{
		const int32 OpenError = errno;
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[Yoko.Guo] MiscInterface Lua crash stack fopen failed errno=%d Path=%s"), OpenError, *LuaCallStackLogPath);
		return;
	}

	const FString Header = FString::Printf(TEXT("\n[LuaCrashStack] Platform=%s Signal=%d\n"), PlatformName, Signal);
	const FTCHARToUTF8 HeaderUtf8(*Header);
	fwrite(HeaderUtf8.Get(), sizeof(char), HeaderUtf8.Length(), FilePtr);

	const FTCHARToUTF8 StackUtf8(*LuaCallStack);
	fwrite(StackUtf8.Get(), sizeof(char), StackUtf8.Length(), FilePtr);
	fclose(FilePtr);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscInterfaceModule, MiscInterface)
