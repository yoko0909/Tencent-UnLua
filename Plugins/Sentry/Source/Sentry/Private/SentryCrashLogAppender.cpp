// Copyright (c) 2022 Sentry. All Rights Reserved.

#include "SentryCrashLogAppender.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

#include "UnLuaModule.h"
#include "UnLuaDebugBase.h"

#include <errno.h>

FString GExtLogFullPath = "";

void AddLuaInfoToLog(const char* LogFileFullName)
{
    FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d"), __LINE__);
    
    UnLua::FLuaEnv* EnvPtr = IUnLuaModule::Get().GetEnv();
    if (nullptr != EnvPtr)
    {
        FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d"), __LINE__);
        
        lua_State* L = EnvPtr->GetMainState();
        if (nullptr != L) 
        {
            FString LogStr = UnLua::GetLuaCallStack(L);
			
            /*
            FString FileStr;
            FFileHelper::LoadFileToString(FileStr, *GExtLogFullPath);
            FileStr.Append(LogStr);
            FFileHelper::SaveStringToFile(FileStr, *GExtLogFullPath);
            */
            
            FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d LogFileFullName is %s"), __LINE__, UTF8_TO_TCHAR(LogFileFullName));
			
            FILE* FilePtr = fopen(LogFileFullName, "ab+");
            if (nullptr != FilePtr)
            {
                FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d"), __LINE__);
                
                int32 LogLen = LogStr.Len();
                if (LogLen > 8)
                {
                    //UTF16CHAR* Log16Ptr = TCHAR_TO_UTF16(*LogStr);
                    //fwrite(Log16Ptr, LogLen, sizeof(UTF16CHAR), FilePtr);

                    //const TCHAR* TCHARData = *LogStr;
                    //fwrite(TCHARData, LogLen, sizeof(TCHAR), FilePtr);
                    
                    FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d LogStr is %s"), __LINE__, *LogStr);
                    
                    FTCHARToUTF8 LogUtf8(*LogStr);
                    const size_t Written = fwrite(LogUtf8.Get(), sizeof(char), LogUtf8.Length(), FilePtr);

                    //const char* LogPtr = TCHAR_TO_ANSI(*LogStr);
                    //fwrite(LogPtr, LogLen, 1, FilePtr);
                    
                    FPlatformMisc::LowLevelOutputDebugStringf(
                        TEXT("[Yoko.Guo] LuaLog Write Len=%d Written=%d"),
                        LogUtf8.Length(),
                        static_cast<int32>(Written)
                    );
                }

                fclose(FilePtr);
            }
            else
            {
                const int32 OpenError = errno;
                FPlatformMisc::LowLevelOutputDebugStringf(
                    TEXT("[Yoko.Guo] AddLuaInfoToLog : Line=%d fopen failed, errno=%d"),
                    __LINE__,
                    OpenError
                );
            }
			
        }
    }
}

void FSentryCrashLogAppender::InitExtLogFullPath()
{
    FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] InitExtLogFullPath : Line=%d"), __LINE__);
    
    const FString LogDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Logs"));
    const bool bCreatedLogDir = IFileManager::Get().MakeDirectory(*LogDir, true);
    FPlatformMisc::LowLevelOutputDebugStringf(
        TEXT("[Yoko.Guo] InitExtLogFullPath : Line=%d LogDir=%s MakeDirectory=%d"),
        __LINE__,
        *LogDir,
        bCreatedLogDir ? 1 : 0
    );
    
    GExtLogFullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*FPaths::Combine(LogDir, TEXT("YokoCrashExtra.log")));
    
    FILE* FilePtr = fopen(TCHAR_TO_UTF8(*GExtLogFullPath), "ab+");
    if (nullptr != FilePtr)
    {
        FPlatformMisc::LowLevelOutputDebugStringf(
            TEXT("[Yoko.Guo] InitExtLogFullPath : Line=%d touched GExtLogFullPath=%s"),
            __LINE__,
            *GExtLogFullPath
        );
        fclose(FilePtr);
    }
    else
    {
        const int32 OpenError = errno;
        FPlatformMisc::LowLevelOutputDebugStringf(
            TEXT("[Yoko.Guo] InitExtLogFullPath : Line=%d touch failed, errno=%d, GExtLogFullPath=%s"),
            __LINE__,
            OpenError,
            *GExtLogFullPath
        );
    }
    
    FString logPath = FGenericPlatformOutputDevices::GetAbsoluteLogFilename();
    FString logFullPath = FPaths::ConvertRelativePathToFull(FGenericPlatformOutputDevices::GetAbsoluteLogFilename());
    FString logFullPath2 = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FGenericPlatformOutputDevices::GetAbsoluteLogFilename());
    
    
    
    // GExtLogFullPath = logFullPath2;
    UE_LOG(LogTemp, Log, TEXT("[Yoko.Guo] FSentryCrashLogAppender: Line=%d, LogPath=%s, LogFullPath=%s, logFullPath2=%s, GExtLogFullPath=%s"), __LINE__, *logPath, *logFullPath, *logFullPath2, *GExtLogFullPath);
}

void FSentryCrashLogAppender::AppendCrashInfo(const TCHAR* PlatformName, int Signal)
{
    FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] InitExtLogFullPath : Line=%d"), __LINE__);
    
    AddLuaInfoToLog(TCHAR_TO_UTF8(*GExtLogFullPath));
}
