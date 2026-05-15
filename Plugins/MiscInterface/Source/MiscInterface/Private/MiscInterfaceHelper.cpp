
#include "MiscInterfaceHelper.h"
#include "MiscInterface.h"
#include "UnLuaModule.h"
#include "UnLuaDebugBase.h"

UMiscInterfaceHelper::UMiscInterfaceHelper(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

//static FAutoConsoleCommand CMD_Misc_Stack(
//    TEXT("Misc.Stack"),
//    TEXT("")
//    TEXT("")
//    TEXT(""),
//    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
//        {
//            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Stack : Line=%d Start"), __LINE__);
//
//            FString ScriptTrace;
//            constexpr int MAX_DEPTH = 64;
//            uint64 StackTrace[MAX_DEPTH] = { 0 };
//            int32 StackTraceDepth = 0;
//            uint32 ProcessId = 0;
//            StackTraceDepth = FPlatformStackWalk::CaptureStackBackTrace(StackTrace, MAX_DEPTH);
//
//            for (int i = 0; i < StackTraceDepth; i++)
//            {
//                UE_LOG(LogMemory, Log, TEXT("0x%llx"), StackTrace[i]);
//            }
//            UE_LOG(LogMemory, Log, TEXT("\n\n"));
//
//            for (int i = 0; i < StackTraceDepth; i++)
//            {
//                ANSICHAR TempString[1024] = { 0 };
//                FPlatformStackWalk::ProgramCounterToHumanReadableString(i, StackTrace[i], TempString, UE_ARRAY_COUNT(TempString));
//
//                UE_LOG(LogMemory, Log, TEXT("%s"), ANSI_TO_TCHAR(TempString));
//            }
//
//            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Stack : Line=%d End"), __LINE__);
//        }),
//    ECVF_Default);

static FAutoConsoleCommand CMD_Misc_Fatal(
    TEXT("Misc.Fatal"),
    TEXT("")
    TEXT("")
    TEXT(""),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] CMD_Misc_Fatal : Line=%d Start"), __LINE__);
            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Fatal : Line=%d Start"), __LINE__);
            UE_LOG(LogMemory, Fatal, TEXT("[Steven.Han] CMD_Misc_Fatal : Line=%d"), __LINE__);
            FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] CMD_Misc_Fatal : Line=%d End"), __LINE__);
            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Fatal : Line=%d End"), __LINE__);
        }),
    ECVF_Default);

static FAutoConsoleCommand CMD_Misc_Abort(
    TEXT("Misc.Abort"),
    TEXT("")
    TEXT("")
    TEXT(""),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] CMD_Misc_Abort : Line=%d Start"), __LINE__);
            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Abort : Line=%d Start"), __LINE__);
            // FGenericPlatformMisc::Abort();
            abort();
            FPlatformMisc::LowLevelOutputDebugStringf( TEXT("[Yoko.Guo] CMD_Misc_Abort : Line=%d End"), __LINE__);
            UE_LOG(LogMemory, Log, TEXT("[Steven.Han] CMD_Misc_Abort : Line=%d End"), __LINE__);
        }),
    ECVF_Default);

static FAutoConsoleCommand CMD_Misc_Lua(
    TEXT("Misc.Lua"),
    TEXT("")
    TEXT("")
    TEXT(""),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            UE_LOG(LogTemp, Display, TEXT("[Yoko.Guo] CMD_Misc.Lua : Line=%d Start"), __LINE__);
            
            UnLua::FLuaEnv* EnvPtr = IUnLuaModule::Get().GetEnv();
            if (nullptr != EnvPtr)
            {
                lua_State* L = EnvPtr->GetMainState();
                if (nullptr != L)
                {
                    FString LogStr = UnLua::GetLuaCallStack(L);
                    
                    UE_LOG(LogTemp, Display, TEXT("[Yoko.Guo] CMD_Misc : Line=%d LogStr is %s"), __LINE__, *LogStr);
                }
            }
        
        }),
    ECVF_Default);


