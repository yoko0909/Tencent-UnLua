// Copyright (c) 2022 Sentry. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSentryCrashLogAppender
{
public:
	static void InitExtLogFullPath();
	static void AppendCrashInfo(const TCHAR* PlatformName, int Signal);
};
