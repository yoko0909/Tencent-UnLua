// Copyright (c) 2022 Sentry. All Rights Reserved.

#include "SentryCrashLogAppender.h"

namespace
{
FSentryCrashInfoInitDelegate GCrashInfoInitDelegate;
FSentryCrashInfoAppendDelegate GCrashInfoAppendDelegate;
}

FDelegateHandle FSentryCrashLogAppender::RegisterCrashInfoInitializer(const FSentryCrashInfoInitDelegate::FDelegate& Delegate)
{
	return GCrashInfoInitDelegate.Add(Delegate);
}

void FSentryCrashLogAppender::UnregisterCrashInfoInitializer(FDelegateHandle Handle)
{
	if (Handle.IsValid())
	{
		GCrashInfoInitDelegate.Remove(Handle);
	}
}

FDelegateHandle FSentryCrashLogAppender::RegisterCrashInfoAppender(const FSentryCrashInfoAppendDelegate::FDelegate& Delegate)
{
	return GCrashInfoAppendDelegate.Add(Delegate);
}

void FSentryCrashLogAppender::UnregisterCrashInfoAppender(FDelegateHandle Handle)
{
	if (Handle.IsValid())
	{
		GCrashInfoAppendDelegate.Remove(Handle);
	}
}

void FSentryCrashLogAppender::InitExtLogFullPath()
{
	GCrashInfoInitDelegate.Broadcast();
}

void FSentryCrashLogAppender::AppendCrashInfo(const TCHAR* PlatformName, int32 Signal)
{
	GCrashInfoAppendDelegate.Broadcast(PlatformName, Signal);
}
