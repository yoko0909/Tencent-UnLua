#include "LifecycleDemoParentWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

namespace
{
	const FName DemoWidgetName(TEXT("LifecycleDemoMain"));

	TStrongObjectPtr<UClass> GParentClass;
	TStrongObjectPtr<ULifecycleDemoParentWidget> GStrongParent;

	UObject* GetCreateOuter(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}

		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance;
		}

		return World;
	}

	void LogCommand(const TCHAR* Event, const FString& Details = FString())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UILifecycleDemo][Command] event=%s details=%s"),
			Event,
			*Details);
	}

	void DumpObject(const TCHAR* Label, const UObject* Object)
	{
		if (!Object)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UILifecycleDemo][Dump] %s=None"), Label);
			return;
		}

		const UObject* Outer = Object->GetOuter();
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UILifecycleDemo][Dump] %s this=%p class=%s name=%s path=%s outer=%s outerPath=%s flags=0x%08x internalFlags=0x%08x"),
			Label,
			Object,
			*GetNameSafe(Object->GetClass()),
			*Object->GetName(),
			*Object->GetPathName(),
			*GetNameSafe(Outer),
			Outer ? *Outer->GetPathName() : TEXT("None"),
			static_cast<uint32>(Object->GetFlags()),
			static_cast<uint32>(Object->GetInternalFlags()));
	}

	void DumpParent(const ULifecycleDemoParentWidget* Parent)
	{
		DumpObject(TEXT("Parent"), Parent);

		if (!Parent)
		{
			return;
		}

		DumpObject(TEXT("Parent.WidgetTree"), Parent->WidgetTree);
		DumpObject(TEXT("Parent.RootWidget"), Parent->WidgetTree ? Parent->WidgetTree->RootWidget : nullptr);
		DumpObject(TEXT("Parent.ChildWidget"), Parent->ChildWidget);
	}

	UClass* LoadParentClass(const TArray<FString>& Args)
	{
		if (Args.Num() > 0)
		{
			UClass* LoadedClass = StaticLoadClass(
				ULifecycleDemoParentWidget::StaticClass(),
				nullptr,
				*Args[0]);

			if (!LoadedClass)
			{
				LogCommand(
					TEXT("LoadClassFailed"),
					FString::Printf(TEXT("path=%s"), *Args[0]));
				return nullptr;
			}

			if (!LoadedClass->IsChildOf(ULifecycleDemoParentWidget::StaticClass()))
			{
				LogCommand(
					TEXT("InvalidClass"),
					FString::Printf(TEXT("class=%s"), *LoadedClass->GetPathName()));
				return nullptr;
			}

			GParentClass.Reset(LoadedClass);
			LogCommand(
				TEXT("ClassLoaded"),
				FString::Printf(TEXT("class=%s"), *LoadedClass->GetPathName()));
		}

		return GParentClass.Get();
	}

	void HandleCreate(const TArray<FString>& Args, UWorld* World)
	{
		if (!World)
		{
			LogCommand(TEXT("CreateFailed"), TEXT("World=None"));
			return;
		}

		UClass* ParentClass = LoadParentClass(Args);
		if (!ParentClass)
		{
			LogCommand(
				TEXT("CreateFailed"),
				TEXT("Pass the Blueprint class path on the first call."));
			return;
		}

		UObject* Outer = GetCreateOuter(World);
		UObject* Existing = FindObjectFast<UObject>(Outer, DemoWidgetName);
		DumpObject(TEXT("BeforeCreate.ExistingSameOuterAndName"), Existing);

		LogCommand(
			TEXT("CreateBegin"),
			FString::Printf(
				TEXT("world=%s outer=%s name=%s class=%s"),
				*World->GetPathName(),
				*GetNameSafe(Outer),
				*DemoWidgetName.ToString(),
				*ParentClass->GetPathName()));

		ULifecycleDemoParentWidget* NewParent =
			CreateWidget<ULifecycleDemoParentWidget>(World, ParentClass, DemoWidgetName);

		if (!NewParent)
		{
			LogCommand(TEXT("CreateFailed"), TEXT("CreateWidget returned None."));
			return;
		}

		GStrongParent.Reset(NewParent);
		NewParent->AddToViewport();
		DumpParent(NewParent);

		LogCommand(
			TEXT("CreateEnd"),
			FString::Printf(TEXT("parent=%p"), NewParent));
	}

	void HandleClose(const TArray<FString>& Args, UWorld* World)
	{
		(void)Args;
		(void)World;

		if (!GStrongParent.IsValid())
		{
			LogCommand(TEXT("CloseSkipped"), TEXT("No strong parent reference."));
			return;
		}

		DumpParent(GStrongParent.Get());
		GStrongParent->RemoveFromParent();
		LogCommand(
			TEXT("CloseEnd"),
			TEXT("RemoveFromParent called; strong reference is intentionally retained."));
	}

	void HandleRelease(const TArray<FString>& Args, UWorld* World)
	{
		(void)Args;
		(void)World;

		if (GStrongParent.IsValid())
		{
			GStrongParent->RemoveFromParent();
		}

		GStrongParent.Reset();
		LogCommand(
			TEXT("ReleaseEnd"),
			TEXT("Viewport reference removed and demo strong reference released."));
	}

	void HandleDump(const TArray<FString>& Args, UWorld* World)
	{
		(void)Args;
		(void)World;

		DumpParent(GStrongParent.Get());
		DumpObject(TEXT("LoadedParentClass"), GParentClass.Get());
	}

	void HandleGC(const TArray<FString>& Args, UWorld* World)
	{
		(void)Args;
		(void)World;

		LogCommand(TEXT("GCBegin"), TEXT("CollectGarbage(RF_NoFlags, true)"));
		CollectGarbage(RF_NoFlags, true);
		LogCommand(
			TEXT("GCEnd"),
			FString::Printf(
				TEXT("strongParentValid=%s"),
				GStrongParent.IsValid() ? TEXT("true") : TEXT("false")));
	}

	static FAutoConsoleCommandWithWorldAndArgs CreateCommand(
		TEXT("UI.LifecycleDemo.Create"),
		TEXT("Create the parent widget. First call requires the Blueprint class path."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleCreate));

	static FAutoConsoleCommandWithWorldAndArgs CloseCommand(
		TEXT("UI.LifecycleDemo.Close"),
		TEXT("Remove the parent from the viewport but retain a strong reference."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleClose));

	static FAutoConsoleCommandWithWorldAndArgs ReleaseCommand(
		TEXT("UI.LifecycleDemo.Release"),
		TEXT("Remove the parent from the viewport and release the demo strong reference."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleRelease));

	static FAutoConsoleCommandWithWorldAndArgs DumpCommand(
		TEXT("UI.LifecycleDemo.Dump"),
		TEXT("Dump parent, WidgetTree, root widget, child widget, and loaded class."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleDump));

	static FAutoConsoleCommandWithWorldAndArgs GCCommand(
		TEXT("UI.LifecycleDemo.GC"),
		TEXT("Run a full synchronous garbage collection."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGC));
}
