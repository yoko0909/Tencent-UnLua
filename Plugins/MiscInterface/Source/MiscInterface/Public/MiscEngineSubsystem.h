// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "MiscEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MISCINTERFACE_API UMiscEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem
	
private:
	void RegisterGCCallbacks();
	void UnregisterGCCallbacks();
	
	void OnPreGarbageCollect();//1.
	void OnPostReachabilityAnalysis();//2.
	void OnPostGarbageCollect();//5.
	void OnPreGarbageCollectConditionalBeginDestroy();//3.
	void OnPostGarbageCollectConditionalBeginDestroy();//4.
	
	FDelegateHandle PreGCDelegateHandle;
	FDelegateHandle PostReachabilityAnalysisHandle;
	FDelegateHandle PostGCDelegateHandle;
	FDelegateHandle PreGCConditionalBeginDestroyHandle;
	FDelegateHandle PostGCConditionalBeginDestroyHandle;
};