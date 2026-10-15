#pragma once

#include "Blueprint/UserWidget.h"

#include "LifecycleDemoChildWidget.generated.h"

UCLASS(Blueprintable)
class UILIFECYCLEDEMO_API ULifecycleDemoChildWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual bool Initialize() override;
	virtual void BeginDestroy() override;
	virtual void FinishDestroy() override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
