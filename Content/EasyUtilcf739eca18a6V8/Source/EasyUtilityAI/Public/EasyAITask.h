// Easy Utility AI - (C) 2025 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIController.h"
#include "EasyAITaskManager.h"
#include "EasyAITask.generated.h"

class UAIUtilityFunctions;

UENUM(BlueprintType)
enum class ETaskType : uint8
{
	/* Can be overridden by a higher scoring task */
	Default,	

	/* Requires explicit Finish call before it can be overriden */
	Explicit, 
};

UCLASS(Blueprintable, BlueprintType, abstract, hidecategories = Object)
class EASYUTILITYAI_API UEasyAITask : public UObject
{
	GENERATED_BODY()

public:

	bool operator==(const UClass* InClass) const { return GetClass() == InClass; }

	/*The channel of the Task. One Task can be run in parallel per channel. Leave this empty to just use the default channel*/
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FGameplayTag Channel;

	/*The 'Is Channel Active' function will ignore this Task. Useful for idling Tasks that do nothing*/
	UPROPERTY(EditDefaultsOnly, Category = "Default", AdvancedDisplay)
	bool bIgnoreChannelActive;

	/*How to handle finishing the Task*/
	UPROPERTY(EditDefaultsOnly, Category = "Scoring", meta = (DisplayName = "Finish Type"))
	ETaskType TaskType;

	/*How the Task score is calculated.*/
	UPROPERTY(EditDefaultsOnly, Category = "Scoring")
	EScoringType ScoringType = EScoringType::ST_Balanced;

	/*The default Score if the Task is executed with 'Run Utility Task' */
	UPROPERTY(EditDefaultsOnly, Category = "Scoring", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DefaultScore = 1.0f;

	/*Weight is used to prioritize Tasks beyond the normalized score of 1.0. Equal Tasks with higher Weight are prioritized */
	UPROPERTY(EditDefaultsOnly, Category = "Scoring", AdvancedDisplay, meta = (ClampMin = "0.1", ClampMax = "10.0", UIMin = "0.1", UIMax = "10.0"))
	float Weight = 1.0f;

	/*How long the tasks needs to be active until it can be overriden*/
	UPROPERTY(EditDefaultsOnly, Category = "Scoring", AdvancedDisplay, meta = (EditCondition = "TaskType == ETaskType::Default"))
	float MinActiveTime = 0;

	/*How long the task can be active before auto finishing*/
	UPROPERTY(EditDefaultsOnly, Category = "Scoring", AdvancedDisplay, meta = (EditCondition = "TaskType == ETaskType::Default"))
	float MaxActiveTime = 0;

	/*The current normalized(non-weighted) score of the Task*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task", meta = (DisplayName = "Score"))
	float ActualScore;

	/*If the Task was active once*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	bool bWasActiveOnce;

	/*The time the Task is active*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	float ElapsedTime;

	/*The time since the Task was last finished*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	float LastFinishTime;

	/*How often the Task was called consecutively*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	int ConsecutiveCount;

	/*How often the Task was active overall*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	int OverallCount;

	/*The associated Task Manager that spawned the Task*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	TObjectPtr<UEasyAITaskManager> Manager;

	/*The owning AI Controller of the Task Manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	TObjectPtr<AAIController> AIOwner;

	/*The owning Pawn or Pawn of the owning AI Controller from the Task Manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task")
	TObjectPtr<APawn> AIOwnerPawn;

protected:

	UPROPERTY()
	TObjectPtr<UWorld> World;

	float FinishTimeStamp;
	float CooldownTime;

	uint8 bIsActive : 1;
	uint8 bIsUpdating : 1;

	float Score;

public:

	/*The channel*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE FGameplayTag GetChannel() const
	{
		return Channel;
	}

	/*If this is a forced task*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	virtual bool IsForced() const
	{
		return false;
	}

	/*If the task is currently in cooldown*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE bool IsDelayed() const
	{
		return !bIsActive && CooldownTime > 0 && FinishTimeStamp > 0 && LastFinishTime < CooldownTime;
	}

	/*If this task needs to be explicitly finished*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE bool IsExplicitFinish() const
	{
		return TaskType == ETaskType::Explicit;
	}

	/*If the task time is currently below the minimum active time*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE bool IsBelowMinActiveTime() const
	{
		return ElapsedTime < MinActiveTime;
	}

	/*If the task is currently active*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE bool IsActive() const
	{
		return bIsActive;
	}

	/*If the task is currently calling it's update function*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	FORCEINLINE bool IsUpdating() const
	{
		return bIsUpdating;
	}

	FORCEINLINE bool IsValidForScoring(const FGameplayTag& TaskChannel) const
	{
		return IsUpdating() && !IsForced() && Score > 0 && TaskChannel == Channel && !IsDelayed();
	}

public:

	/*Finishes the task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task", meta = (BlueprintProtected))
	void FinishTask();

	/*Get a Task based on a specific class*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task", meta = (DeterminesOutputType = "TaskClass"))
	UEasyAITask* GetTask(TSubclassOf<UEasyAITask> TaskClass) const;

	/*Get the currently active Task*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task")
	UEasyAITask* GetActiveTask() const;

	/*Check to see if a Task-Channel currently has an active Task*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager")
	bool IsChannelActive(FGameplayTag ActiveChannel) const;

	/*This gets called once the Task is spawned*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task", meta = (DisplayName = "Begin Play"))
	void OnBeginPlay();

	/*The Tick event is called while the Task is active*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task", meta = (DisplayName = "Tick"))
	void OnTick(float DeltaTime);

	/*This is called when the Task becomes active*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task", meta = (DisplayName = "Start"))
	void OnStart(UEasyAITask* PreviousTask);

	/*Called when the Task finishes*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task")
	void OnFinish();

	/*Called when the Task Manager and it's Tasks are destroyed*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task")
	void OnDestroyed();

	/*Safely returns the score or weighted score of a specific Task. Returns local Score for this Task if the Class input is empty*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task", meta = (AdvancedDisplay = "TaskClass, bWeighted"))
	float GetTaskScore(TSubclassOf<UEasyAIUtilityTask> TaskClass, bool bWeighted) const;

public:

	virtual UWorld* GetWorld() const override;

	float GetScore() const { return Score; };
	float GetWeightedScore(const bool bCached = false) const;

	virtual void TaskDefaultUpdate() {};

	void ReceiveTick(float DeltaTime);
	void InitTask(UEasyAITaskManager* TaskManager);
	void SetTaskActive(UEasyAITask* Previous);

	void InternalFinishTask(const bool bRemoveActiveTask);
};


UCLASS(hidedropdown, hidecategories = (Object, Scoring))
class EASYUTILITYAI_API UEasyAIForcedTask : public UEasyAITask
{
	GENERATED_BODY()

public:

	virtual bool IsForced() const override {return true;}
};

UCLASS(hidedropdown, hidecategories = Object)
class EASYUTILITYAI_API UEasyAIUtilityTask : public UEasyAITask
{
	GENERATED_BODY()

public:

	/*This event is called from the 'Run Utility Task' function and should be used for scoring*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Utility Task", meta = (DisplayName = "Update"))
	void OnScore();

	/*Directly set the score of a Task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Utility Task", meta = (BlueprintProtected))
	void SetScore(float NewScore);

	/*Set a true utility score based on actual scorers*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Utility Task", meta = (BlueprintProtected))
	void SetUtilityScore(TArray<FUtilityScore> Scorers);

	/*A task in cooldown is not considered for scoring*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Utility Task")
	void SetCooldown(float Time = 1.0f);

public:

	virtual void TaskDefaultUpdate() override;

	void UpdateCooldown();
	void UpdateTask();
	void NormalizeScore();
};