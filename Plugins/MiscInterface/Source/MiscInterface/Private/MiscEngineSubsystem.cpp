// Fill out your copyright notice in the Description page of Project Settings.


#include "MiscEngineSubsystem.h"
#include "MiscInterface.h"
#include "Engine/LevelStreaming.h"

static FAutoConsoleCommand CMD_Misc_DumpCrashReport(
	TEXT("Misc.DumpCrashReport"),
	TEXT("DumpCrashReport."),
	FConsoleCommandDelegate::CreateLambda([]
	{
		// ANSICHAR CallStack[1024];
		// FPlatformStackWalk::StackWalkAndDump(CallStack, 1024, 0);
		// UE_LOG(LogMiscInterface, Error, TEXT("%s"), ANSI_TO_TCHAR(CallStack));
		
		// FDebug::DumpStackTraceToLog(ELogVerbosity::Error);
		
		// UE_LOG(LogMiscInterface, Fatal, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, DumpCrashReport"), __LINE__);
	}));

void UMiscEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#if !UE_BUILD_SHIPPING
	RegisterGCCallbacks();
#endif
}

void UMiscEngineSubsystem::Deinitialize()
{
#if !UE_BUILD_SHIPPING
	UnregisterGCCallbacks();
#endif
	
	Super::Deinitialize();
}

void UMiscEngineSubsystem::RegisterGCCallbacks()
{
	PreGCDelegateHandle = FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(this, &UMiscEngineSubsystem::OnPreGarbageCollect);
	
	PostReachabilityAnalysisHandle = FCoreUObjectDelegates::PostReachabilityAnalysis.AddUObject(this, &UMiscEngineSubsystem::OnPostReachabilityAnalysis);
	
	PostGCDelegateHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &UMiscEngineSubsystem::OnPostGarbageCollect);
	
	PreGCConditionalBeginDestroyHandle = FCoreUObjectDelegates::PreGarbageCollectConditionalBeginDestroy.AddUObject(this, &UMiscEngineSubsystem::OnPreGarbageCollectConditionalBeginDestroy);
	
	PostGCConditionalBeginDestroyHandle = FCoreUObjectDelegates::PostGarbageCollectConditionalBeginDestroy.AddUObject(this, &UMiscEngineSubsystem::OnPostGarbageCollectConditionalBeginDestroy);
	
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, RegisterGCCallbacks"), __LINE__);
}

void UMiscEngineSubsystem::UnregisterGCCallbacks()
{
	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().Remove(PreGCDelegateHandle);
	FCoreUObjectDelegates::PostReachabilityAnalysis.Remove(PostReachabilityAnalysisHandle);
	FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGCDelegateHandle);
	FCoreUObjectDelegates::PreGarbageCollectConditionalBeginDestroy.Remove(PreGCConditionalBeginDestroyHandle);
	FCoreUObjectDelegates::PostGarbageCollectConditionalBeginDestroy.Remove(PostGCConditionalBeginDestroyHandle);
	
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, UnregisterGCCallbacks"), __LINE__);
}

void UMiscEngineSubsystem::OnPreGarbageCollect()
{
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, OnPreGarbageCollect"), __LINE__);
	
	const UWorld* curWorld = GEngine->GetCurrentPlayWorld();
	if (curWorld)
	{
		UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Current World: %s"), __LINE__, *curWorld->GetName());
		const ULevel* curLevel = curWorld->GetCurrentLevel();
		if (curLevel)
		{
			UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Current Level: %s"), __LINE__, *curLevel->GetName());
		}
		const TArray<ULevelStreaming*>& streamingLevels = curWorld->GetStreamingLevels();
		if (streamingLevels.Num() > 0)
		{
			for (const ULevelStreaming* Level : streamingLevels)
			{
				if (Level && Level->GetLoadedLevel())
				{
					UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Level: %s State is %s"), __LINE__, *Level->GetWorldAssetPackageName(), Level->IsLevelLoaded() ? TEXT("Loaded") : TEXT("Unloaded"));
				}
			}
		}
	}
	
#if 0
	if (FGCObject::GGCObjectReferencer)
	{
		UGCObjectReferencer* ref = FGCObject::GGCObjectReferencer;
		
		checkf(ref->IsValidLowLevel(), TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, GGCObjectReferencer is invalid"));
		const TArray<FGCObject*>& objects = ref->GetReferencedObjects();
		for (int32 i = 0; i < objects.Num(); ++i)
		{
			if (!objects[i])
			{
				UE_LOG(LogMiscInterface, Error, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Null object found at index %d"), __LINE__, i);
				
				if (i > 0 && objects[i - 1])
				{
					FString refName = objects[i - 1]->GetReferencerName();
					UE_LOG(LogMiscInterface, Error, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Valid object found at index %d, FGCObject is %p, refName is %s"), __LINE__, i - 1, objects[i - 1], *refName);
				}
				if (i < objects.Num() - 1 && objects[i + 1])
				{
					FString refName = objects[i + 1]->GetReferencerName();
					UE_LOG(LogMiscInterface, Error, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Valid object found at index %d, FGCObject is %p, refName is %s"), __LINE__, i + 1, objects[i + 1], *refName);
				}
			}
			else
			{
				FString refName = objects[i]->GetReferencerName();
				UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, Valid object found at index %d, FGCObject is %p, refName is %s"), __LINE__, i, objects[i], *refName);
			}
		}
	}
#endif
}

void UMiscEngineSubsystem::OnPostReachabilityAnalysis()
{
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, OnPostReachabilityAnalysis"), __LINE__);
}

void UMiscEngineSubsystem::OnPostGarbageCollect()
{
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, OnPostGarbageCollect"), __LINE__);
}

void UMiscEngineSubsystem::OnPreGarbageCollectConditionalBeginDestroy()
{
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, OnPreGarbageCollectConditionalBeginDestroy"), __LINE__);
}

void UMiscEngineSubsystem::OnPostGarbageCollectConditionalBeginDestroy()
{
	UE_LOG(LogMiscInterface, Display, TEXT("[Yoko.Guo] MiscEngineSubsystem Line = %d, OnPostGarbageCollectConditionalBeginDestroy"), __LINE__);
}
