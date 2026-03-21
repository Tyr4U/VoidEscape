// Easy Utility AI - (C) 2025 by Michael Hegemann


#include "EasyAISubsystem.h"

UEasyAISubsystem* UEasyAISubsystem::GetCurrent(const UWorld* World)
{
	return UWorld::GetSubsystem<UEasyAISubsystem>(World);
}

UEasyAIKeyStorage* UEasyAISubsystem::CreateNewSharedKeys(UClass* ManagerClass)
{
	if (!SharedKeys.Find(ManagerClass))
	{
		UEasyAIKeyStorage* NewSharedKeys = NewObject<UEasyAIKeyStorage>(this, UEasyAIKeyStorage::StaticClass());
		if (NewSharedKeys)
		{
			SharedKeys.Add(ManagerClass, NewSharedKeys);
			return NewSharedKeys;
		}
	}

	return nullptr;
}

UEasyAIKeyStorage* UEasyAISubsystem::GetSharedKeys(UClass* ManagerClass) const
{
	return SharedKeys.FindRef(ManagerClass);
}
