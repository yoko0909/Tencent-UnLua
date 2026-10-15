#include "LifecycleDemoParentWidget.h"

#include "LifecycleDemoChildWidget.h"
#include "LifecycleDemoLog.h"
#include "Blueprint/WidgetTree.h"

namespace
{
	void LogParentTree(const TCHAR* Event, const ULifecycleDemoParentWidget* Parent)
	{
		if (!Parent)
		{
			UILifecycleDemoLog::ObjectEvent(TEXT("Parent"), Event, nullptr);
			return;
		}

		const UWidget* RootWidget = Parent->WidgetTree ? Parent->WidgetTree->RootWidget : nullptr;
		UILifecycleDemoLog::ObjectEvent(
			TEXT("Parent"),
			Event,
			Parent,
			FString::Printf(
				TEXT("widgetTree=%p root=%p rootName=%s child=%p childPath=%s"),
				Parent->WidgetTree,
				RootWidget,
				*GetNameSafe(RootWidget),
				Parent->ChildWidget,
				Parent->ChildWidget ? *Parent->ChildWidget->GetPathName() : TEXT("None")));
	}
}

void ULifecycleDemoParentWidget::PostInitProperties()
{
	Super::PostInitProperties();
	LogParentTree(TEXT("PostInitProperties"), this);
}

void ULifecycleDemoParentWidget::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	LogParentTree(
		TEXT("PostDuplicate"),
		this);
}

bool ULifecycleDemoParentWidget::Initialize()
{
	const bool bLocalInitialized = Super::Initialize();
	LogParentTree(
		TEXT("Initialize"),
		this);
	return bLocalInitialized;
}

void ULifecycleDemoParentWidget::NativeConstruct()
{
	LogParentTree(TEXT("NativeConstruct"), this);
	Super::NativeConstruct();
}

void ULifecycleDemoParentWidget::NativeDestruct()
{
	LogParentTree(TEXT("NativeDestruct"), this);
	Super::NativeDestruct();
}

void ULifecycleDemoParentWidget::BeginDestroy()
{
	LogParentTree(TEXT("BeginDestroy"), this);
	Super::BeginDestroy();
}

void ULifecycleDemoParentWidget::FinishDestroy()
{
	LogParentTree(TEXT("FinishDestroy"), this);
	Super::FinishDestroy();
}
