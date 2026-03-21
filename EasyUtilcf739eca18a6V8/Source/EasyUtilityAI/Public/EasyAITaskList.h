// Easy Utility AI - (C) 2025 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EasyAITaskList.generated.h"

class UEasyAIUtilityTask;
class UEasyAIForcedTask;

UCLASS(BlueprintType, meta = (DisplayName = "Easy AI Task List"))
class EASYUTILITYAI_API UEasyAITaskList : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/*Run Utility Task is called automatically for all listed Tasks*/
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	bool bAutoRunUtilityTask;

	/*List of Utility Task classes to pre load*/
	UPROPERTY(EditDefaultsOnly, Category = "Default", meta = (DisplayName = "Utility Tasks"))
	TArray<TSubclassOf<UEasyAIUtilityTask>> TaskClasses;

	/*List of Forced Task classes to pre load*/
	UPROPERTY(EditDefaultsOnly, Category = "Default", meta = (DisplayName = "Forced Tasks"))
	TArray<TSubclassOf<UEasyAIForcedTask>> ForcedTaskClasses;
};
