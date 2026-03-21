// Easy Utility AI - (C) 2025 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EngineUtils.h"
#include "EasyAIKeyStorage.h"
#include "EasyAISubsystem.generated.h"


UCLASS()
class EASYUTILITYAI_API UEasyAISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	TMap<UClass*, TObjectPtr<UEasyAIKeyStorage>> SharedKeys;

public:

	static UEasyAISubsystem* GetCurrent(const UWorld* World);

	UEasyAIKeyStorage* CreateNewSharedKeys(UClass* ManagerClass);
	UEasyAIKeyStorage* GetSharedKeys(UClass* ManagerClass) const;
};
