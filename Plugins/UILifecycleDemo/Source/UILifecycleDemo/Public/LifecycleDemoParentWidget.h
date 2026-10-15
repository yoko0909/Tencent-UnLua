#pragma once

#include "Blueprint/UserWidget.h"

#include "LifecycleDemoParentWidget.generated.h"

class ULifecycleDemoChildWidget;

UCLASS(Blueprintable)
class UILIFECYCLEDEMO_API ULifecycleDemoParentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual bool Initialize() override;
	virtual void BeginDestroy() override;
	virtual void FinishDestroy() override;

	/** The child user widget placed in the parent Widget Blueprint Designer. */
	UPROPERTY(meta = (BindWidget))
	ULifecycleDemoChildWidget* ChildWidget;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
