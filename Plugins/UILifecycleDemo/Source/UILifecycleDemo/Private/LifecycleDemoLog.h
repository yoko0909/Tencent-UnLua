#pragma once

#include "CoreMinimal.h"

namespace UILifecycleDemoLog
{
	inline void ObjectEvent(const TCHAR* Kind, const TCHAR* Event, const UObject* Object, const FString& Extra = FString())
	{
		if (!Object)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[UILifecycleDemo][%s] event=%s object=None extra=%s"),
				Kind,
				Event,
				*Extra);
			return;
		}

		const UObject* Outer = Object->GetOuter();
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UILifecycleDemo][%s] event=%s this=%p class=%s name=%s path=%s outer=%s outerPath=%s flags=0x%08x internalFlags=0x%08x extra=%s"),
			Kind,
			Event,
			Object,
			*GetNameSafe(Object->GetClass()),
			*Object->GetName(),
			*Object->GetPathName(),
			*GetNameSafe(Outer),
			Outer ? *Outer->GetPathName() : TEXT("None"),
			static_cast<uint32>(Object->GetFlags()),
			static_cast<uint32>(Object->GetInternalFlags()),
			*Extra);
	}
}
