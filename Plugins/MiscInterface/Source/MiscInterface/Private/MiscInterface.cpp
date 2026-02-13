// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscInterface.h"

DEFINE_LOG_CATEGORY(LogMiscInterface);

#define LOCTEXT_NAMESPACE "FMiscInterfaceModule"

void FMiscInterfaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMiscInterfaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscInterfaceModule, MiscInterface)