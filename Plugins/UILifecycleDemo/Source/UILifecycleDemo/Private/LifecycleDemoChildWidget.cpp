#include "LifecycleDemoChildWidget.h"

#include "LifecycleDemoLog.h"

void ULifecycleDemoChildWidget::PostInitProperties()
{
	Super::PostInitProperties();
	UILifecycleDemoLog::ObjectEvent(TEXT("Child"), TEXT("PostInitProperties"), this);
}

void ULifecycleDemoChildWidget::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	UILifecycleDemoLog::ObjectEvent(
		TEXT("Child"),
		TEXT("PostDuplicate"),
		this,
		FString::Printf(TEXT("duplicateForPIE=%s"), bDuplicateForPIE ? TEXT("true") : TEXT("false")));
}

bool ULifecycleDemoChildWidget::Initialize()
{
	const bool bLocalInitialized = Super::Initialize();
	UILifecycleDemoLog::ObjectEvent(
		TEXT("Child"),
		TEXT("Initialize"),
		this,
		FString::Printf(TEXT("result=%s"), bLocalInitialized ? TEXT("true") : TEXT("false")));
	return bLocalInitialized;
}

void ULifecycleDemoChildWidget::NativeConstruct()
{
	UILifecycleDemoLog::ObjectEvent(TEXT("Child"), TEXT("NativeConstruct"), this);
	Super::NativeConstruct();
}

void ULifecycleDemoChildWidget::NativeDestruct()
{
	UILifecycleDemoLog::ObjectEvent(TEXT("Child"), TEXT("NativeDestruct"), this);
	Super::NativeDestruct();
}

void ULifecycleDemoChildWidget::BeginDestroy()
{
	UILifecycleDemoLog::ObjectEvent(TEXT("Child"), TEXT("BeginDestroy"), this);
	Super::BeginDestroy();
}

void ULifecycleDemoChildWidget::FinishDestroy()
{
	UILifecycleDemoLog::ObjectEvent(TEXT("Child"), TEXT("FinishDestroy"), this);
	Super::FinishDestroy();
}
