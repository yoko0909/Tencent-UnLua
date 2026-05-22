// Copyright (c) 2022 Sentry. All Rights Reserved.

#include "SentryAppleCrashHook.h"

#include "SentryCrashLogAppender.h"

#include "HAL/Platform.h"

#if PLATFORM_IOS

#include <os/log.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#ifndef SENTRY_APPLE_CRASH_USE_SENTRYCRASH_CALLBACK
#define SENTRY_APPLE_CRASH_USE_SENTRYCRASH_CALLBACK 1
#endif

namespace
{
static volatile sig_atomic_t GHookInstalled = 0;

static os_log_t GetSentryAppleCrashHookLog()
{
	static os_log_t HookLog = os_log_create("com.hero.dna", "SentryCrashHook");
	return HookLog;
}

static void WriteDiagnosticLine(const char* Message, unsigned long Length)
{
	if (Message == nullptr || Length == 0)
	{
		return;
	}

	(void)write(STDERR_FILENO, Message, Length);
}

#if SENTRY_APPLE_CRASH_USE_SENTRYCRASH_CALLBACK

static constexpr char SentryCrashCallbackMessage[] = "[Yoko.Guo] Entered iOS SentryCrash diagnostic callback\n";
static constexpr char SentryCrashCallbackInstallMessage[] = "[Yoko.Guo] Installed iOS SentryCrash diagnostic callback\n";

struct SentryCrash_MonitorContext;
typedef void (*SentryCrashMonitorEventCallback)(struct SentryCrash_MonitorContext* MonitorContext);

extern "C" SentryCrashMonitorEventCallback sentrycrashcm_getEventCallback(void);
extern "C" void sentrycrashcm_setEventCallback(SentryCrashMonitorEventCallback Callback);

static SentryCrashMonitorEventCallback GPreviousSentryCrashCallback = nullptr;

static void DiagnosticSentryCrashCallback(SentryCrash_MonitorContext* MonitorContext)
{
	WriteDiagnosticLine(SentryCrashCallbackMessage, sizeof(SentryCrashCallbackMessage) - 1);
	FSentryCrashLogAppender::AppendCrashInfo(TEXT("IOS"), 0);

	if (GPreviousSentryCrashCallback != nullptr)
	{
		GPreviousSentryCrashCallback(MonitorContext);
	}
}

static void InstallSentryCrashCallbackHook()
{
	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installing iOS SentryCrash diagnostic callback");
	WriteDiagnosticLine(SentryCrashCallbackInstallMessage, sizeof(SentryCrashCallbackInstallMessage) - 1);

	GPreviousSentryCrashCallback = sentrycrashcm_getEventCallback();
	sentrycrashcm_setEventCallback(DiagnosticSentryCrashCallback);

	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installed iOS SentryCrash diagnostic callback");
}

#endif

#if !SENTRY_APPLE_CRASH_USE_SENTRYCRASH_CALLBACK

static constexpr char SignalFallbackMessage[] = "[Yoko.Guo] Entered iOS crash signal diagnostic hook\n";
static constexpr char SignalFallbackInstallMessage[] = "[Yoko.Guo] Installed iOS crash signal diagnostic hook\n";

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
};

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

static void ForwardToPreviousSignalAction(int Signal, siginfo_t* Info, void* Context)
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

static void DiagnosticSignalHandler(int Signal, siginfo_t* Info, void* Context)
{
	WriteDiagnosticLine(SignalFallbackMessage, sizeof(SignalFallbackMessage) - 1);
	FSentryCrashLogAppender::AppendCrashInfo(TEXT("IOS"), Signal);
	ForwardToPreviousSignalAction(Signal, Info, Context);
}

static void InstallSignalFallbackHook()
{
	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installing iOS crash signal diagnostic hook");
	WriteDiagnosticLine(SignalFallbackInstallMessage, sizeof(SignalFallbackInstallMessage) - 1);

	struct sigaction Action;
	memset(&Action, 0, sizeof(Action));
	sigemptyset(&Action.sa_mask);
	Action.sa_sigaction = DiagnosticSignalHandler;
	Action.sa_flags = SA_SIGINFO | SA_ONSTACK;

	for (int Index = 0; Index < static_cast<int>(sizeof(GSignalHookStates) / sizeof(GSignalHookStates[0])); ++Index)
	{
		sigaction(GSignalHookStates[Index].Signal, &Action, &GSignalHookStates[Index].PreviousAction);
	}

	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installed iOS crash signal diagnostic hook");
}

#endif
} // namespace

void InstallSentryAppleCrashHook()
{
	if (GHookInstalled != 0)
	{
		os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] iOS Sentry crash hook already installed");
		return;
	}

	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installing iOS Sentry crash hook");
	FSentryCrashLogAppender::InitExtLogFullPath();

#if SENTRY_APPLE_CRASH_USE_SENTRYCRASH_CALLBACK
	InstallSentryCrashCallbackHook();
#else
	InstallSignalFallbackHook();
#endif

	GHookInstalled = 1;
	os_log_info(GetSentryAppleCrashHookLog(), "[Yoko.Guo] Installed iOS Sentry crash hook");
}

#else

void InstallSentryAppleCrashHook()
{
}

#endif
