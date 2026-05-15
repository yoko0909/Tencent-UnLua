// Copyright (c) 2022 Sentry. All Rights Reserved.

#include <android/log.h>
#include <signal.h>
#include <string.h>
#include "SentryCrashLogAppender.h"

namespace
{
constexpr const char* HookLogTag = "YokoCrashHook";

struct FSignalHookState
{
	int Signal;
	struct sigaction PreviousAction;
};

static FSignalHookState GSignalHookStates[] = {
	{ SIGABRT, {} },
	{ SIGSEGV, {} },
	{ SIGBUS, {} },
	{ SIGILL, {} },
	{ SIGFPE, {} },
	{ SIGTRAP, {} },
#if defined(SIGSYS)
	{ SIGSYS, {} },
#endif
};

static volatile sig_atomic_t GHookInstalled = 0;

static FSignalHookState* FindSignalHookState(int Signal)
{
	for (int Index = 0; Index < static_cast<int>(sizeof(GSignalHookStates) / sizeof(GSignalHookStates[0])); ++Index)
	{
		if (GSignalHookStates[Index].Signal == Signal)
		{
			return &GSignalHookStates[Index];
		}
	}

	return nullptr;
}

static char* AppendLiteral(char* Cursor, const char* Literal)
{
	while (*Literal != '\0')
	{
		*Cursor++ = *Literal++;
	}

	return Cursor;
}

static char* AppendInt(char* Cursor, int Value)
{
	if (Value == 0)
	{
		*Cursor++ = '0';
		return Cursor;
	}

	if (Value < 0)
	{
		*Cursor++ = '-';
		Value = -Value;
	}

	char Digits[16];
	int Count = 0;
	while (Value > 0 && Count < static_cast<int>(sizeof(Digits)))
	{
		Digits[Count++] = static_cast<char>('0' + (Value % 10));
		Value /= 10;
	}

	while (Count > 0)
	{
		*Cursor++ = Digits[--Count];
	}

	return Cursor;
}

static void LogCrashSignalEntry(int Signal)
{
	char Message[96];
	char* Cursor = Message;
	Cursor = AppendLiteral(Cursor, "[Yoko.Guo] Entered custom crash signal hook, signal=");
	Cursor = AppendInt(Cursor, Signal);
	*Cursor = '\0';

	__android_log_write(ANDROID_LOG_INFO, HookLogTag, Message);
}

static void ForwardToPreviousHandler(int Signal, siginfo_t* Info, void* Context)
{
	FSignalHookState* State = FindSignalHookState(Signal);
	if (State == nullptr)
	{
		signal(Signal, SIG_DFL);
		raise(Signal);
		return;
	}

	const struct sigaction& PreviousAction = State->PreviousAction;
	if ((PreviousAction.sa_flags & SA_SIGINFO) != 0 && PreviousAction.sa_sigaction != nullptr)
	{
		PreviousAction.sa_sigaction(Signal, Info, Context);
		return;
	}

	if (PreviousAction.sa_handler == SIG_IGN)
	{
		return;
	}

	if (PreviousAction.sa_handler == SIG_DFL)
	{
		signal(Signal, SIG_DFL);
		raise(Signal);
		return;
	}

	if (PreviousAction.sa_handler != nullptr)
	{
		PreviousAction.sa_handler(Signal);
		return;
	}

	signal(Signal, SIG_DFL);
	raise(Signal);
}

static void CrashSignalHandler(int Signal, siginfo_t* Info, void* Context)
{
	LogCrashSignalEntry(Signal);
	FSentryCrashLogAppender::AppendCrashInfo(TEXT("Android"), Signal);
	ForwardToPreviousHandler(Signal, Info, Context);
}
} // namespace

void InstallSentryCrashSignalHook()
{
	if (GHookInstalled != 0)
	{
		__android_log_write(ANDROID_LOG_INFO, HookLogTag, "[Yoko.Guo] Crash signal hook already installed");
		return;
	}

	struct sigaction Action;
	memset(&Action, 0, sizeof(Action));
	sigemptyset(&Action.sa_mask);
	Action.sa_sigaction = CrashSignalHandler;
	Action.sa_flags = SA_SIGINFO | SA_ONSTACK;

	for (int Index = 0; Index < static_cast<int>(sizeof(GSignalHookStates) / sizeof(GSignalHookStates[0])); ++Index)
	{
		sigaction(GSignalHookStates[Index].Signal, &Action, &GSignalHookStates[Index].PreviousAction);
	}

	GHookInstalled = 1;
	FSentryCrashLogAppender::InitExtLogFullPath();
	__android_log_write(ANDROID_LOG_INFO, HookLogTag, "[Yoko.Guo] Crash signal hook installed");
}
