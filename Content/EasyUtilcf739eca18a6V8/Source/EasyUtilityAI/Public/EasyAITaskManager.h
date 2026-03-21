// Easy Utility AI - (C) 2025 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "AIUtilityData.h"
#include "GameFramework/Character.h"
#include "Runtime/Launch/Resources/Version.h"
#include "EasyAITaskManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEasyAI, Log, All);

class UEasyAITaskGroup;
class UEasyAIKeyStorage;
class UEasyAITaskList;

UCLASS(BlueprintType, Blueprintable, abstract, hidecategories = (Collision, Navigation, Replication, Tags, ComponentReplication, Events, Sockets, Activation, Cooking, AssetUserData, Variable, ComponentTick), ClassGroup=("Easy Utility AI"), meta=(DisplayName = "Easy AI Task Manager", BlueprintSpawnableComponent))
class EASYUTILITYAI_API UEasyAITaskManager : public UActorComponent
{
	GENERATED_BODY()

protected:

	UEasyAITaskManager();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:

	/*All tasks that have been spawned by the manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TSet<TObjectPtr<UEasyAITask>> AllTasks;

	/*If owned by an AI Controller*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TObjectPtr<AAIController> AIOwner;

	/*The Pawn owner or the Pawn of the AI Controller that owns this manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TObjectPtr<APawn> AIOwnerPawn;

	/*The storage object for local runtime keys*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TObjectPtr<UEasyAIKeyStorage> LocalKeys;

	/*The storage object of shared runtime keys*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TObjectPtr<UEasyAIKeyStorage> SharedKeys;

	/*All Task channels that are part of this Task Manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager", meta = (DisplayName = "Task Channels"))
	TSet<FGameplayTag> AllChannels;

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	uint32 TaskListNum = 0;
#endif

	/*All Easy AI Utility Task classes in the Data Assets are added to the Task Manager on Begin Play*/
	UPROPERTY(EditDefaultsOnly, Category = "Task Manager", meta = (DisplayName = "Task Preload Assets"))
	TArray<TSoftObjectPtr<UEasyAITaskList>> TaskLists;

	/*A Tasks needs a score with a min. higher than this value to override another*/
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Task Manager")
	float MinOverrideScore = 0.005;

	/*The rate at which UpdateTasks is called*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Task Manager", meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float TaskUpdateRate = 0.1f;

public:	
	
	/*Get a Task based on a specific class*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager", meta = (DeterminesOutputType = "TaskClass"))
	UEasyAITask* GetTask(TSubclassOf<UEasyAITask> TaskClass) const;

	/*Get the currently active Task of a channel*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager")
	UEasyAITask* GetActiveTask(FGameplayTag Channel) const;

	/*Activates a Forced Task without Scoring*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Run Forced Task"))
	bool ForceTask(TSubclassOf<UEasyAIForcedTask> TaskClass);

	/*Calls the Score function on the Task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Run Utility Task"))
	void RunUtilityTask(TSubclassOf<UEasyAIUtilityTask> TaskClass);

	/*Finishes the currently active Task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Finish Active Task"))
	void FinishActiveTask(FGameplayTag Channel);

	/*Main function to run Tasks from*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task Manager", meta = (DisplayName = "Update Tasks"))
	void OnUpdateTasks();

	/*Check to see if a Task-Channel currently has an active Task.*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager")
	bool IsChannelActive(FGameplayTag Channel) const;

	/*Re-Initializes the Pawn when the Task Manager is attached to an AI Controller. Useful when possessing a new Pawn.*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Reset AI Owner Pawn"))
	void ResetPawnForAiOwner();

public:

	void AddActiveTask(UEasyAITask* TheTask);
	void RemoveActiveTask(UEasyAITask* TheTask);
	
	UPROPERTY()
	TSet<TObjectPtr<UEasyAITaskList>> TaskListPointers;

	UPROPERTY()
	TSet<TSubclassOf<UEasyAITask>> AllTaskClasses;

	UPROPERTY()
	TSet<TObjectPtr<UEasyAITask>> ActiveTasks;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UEasyAITask>> LastFinishedTasks;

private:

	TSharedPtr<FStreamableHandle> StreamableHandle;

	enum ETaskPreloadState
	{
		NoPreload,
		Pending,
		Preloaded,
		Complete,
	};
	ETaskPreloadState TaskPreloadState;

	FTimerHandle CheckTaskAllocationTimer;
	float TaskManagerUpdateTime;

	void CheckTaskAllocation();
	void PreloadTaskLists(const TArray<TSoftObjectPtr<UEasyAITaskList>>& InLists);
	void OnAllTaskListsLoaded();

	void InitKeyManagers();
	void PrecacheTaskClasses(const FString& SearchPath);
	bool AllocateTasks();
	void SelectTask(const FGameplayTag& Channel = DefaultChannelTag);
	UEasyAITask* SpawnNewTask(const TSubclassOf<UEasyAITask>& TaskClass);
	UEasyAITask* GetLastFinishedTask(const FGameplayTag& Channel) const;

	bool CanValidatePawnForAiOwner();

	bool ShouldUpdateTask() const;
	void UpdateTasks();
	void AutoRunUtilityTasks();

	void SetAllocationTimer();

	FORCEINLINE bool HasAnyTasks() const
	{
		return AllTasks.Num() > 0;
	}

	UEasyAITask* CompareScores(const TArray<UEasyAITask*>& Scorers, bool& bHasEquals) const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};
