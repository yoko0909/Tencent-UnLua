// Copyright (c) 2022 Sentry. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE(FSentryCrashInfoInitDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSentryCrashInfoAppendDelegate, const TCHAR* /* PlatformName */, int32 /* Signal */);

// These callbacks may be broadcast from native crash handlers. Listeners must treat
// them as diagnostic-only unless their work is known to be crash-handler safe.
class SENTRY_API FSentryCrashLogAppender
{
public:
	static FDelegateHandle RegisterCrashInfoInitializer(const FSentryCrashInfoInitDelegate::FDelegate& Delegate);
	static void UnregisterCrashInfoInitializer(FDelegateHandle Handle);

	static FDelegateHandle RegisterCrashInfoAppender(const FSentryCrashInfoAppendDelegate::FDelegate& Delegate);
	static void UnregisterCrashInfoAppender(FDelegateHandle Handle);

	static void InitExtLogFullPath();
	static void AppendCrashInfo(const TCHAR* PlatformName, int32 Signal);
};
