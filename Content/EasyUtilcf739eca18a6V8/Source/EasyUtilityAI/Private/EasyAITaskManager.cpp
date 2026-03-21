// Easy Utility AI - (C) 2025 by Michael Hegemann


#include "EasyAITaskManager.h"
#include "Engine/World.h"
#include "Engine/Blueprint.h"
#include "EasyAITask.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "EasyAIQueryObject.h"
#include "AIUtilityFunctions.h"
#include "EasyAIKeyStorage.h"
#include "EasyAITaskList.h"
#include "EasyAISubsystem.h"

DEFINE_LOG_CATEGORY(LogEasyAI);
UE_DEFINE_GAMEPLAY_TAG(DefaultChannelTag, DefaultChannelTagName)

UEasyAITaskManager::UEasyAITaskManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;

	TaskPreloadState = ETaskPreloadState::NoPreload;
}

#if WITH_EDITOR
void UEasyAITaskManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	TaskListNum = TaskLists.Num();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UEasyAITaskManager::BeginPlay()
{
	const UEasyAITaskManager* TaskManComp = GetOwner()->FindComponentByClass<UEasyAITaskManager>();
	if (TaskManComp)
	{
		if (TaskManComp != this)
		{
			UE_LOG(LogEasyAI, Error, TEXT("%s has multiple Task Managers!"), *GetOwner()->GetName());
		}
	}

	AIOwner = Cast<AAIController>(GetOwner());
	if (!AIOwner)
	{
		AIOwnerPawn = Cast<APawn>(GetOwner());
		if (!AIOwnerPawn)
		{
			UE_LOG(LogEasyAI, Warning, TEXT("Task Manager must be attached to AIController or Pawn!"));
			return;
		}
	}

	//Preloading with data assets
	if (!TaskLists.IsEmpty())
	{
		TaskPreloadState = ETaskPreloadState::Pending;
		PreloadTaskLists(TaskLists);
	}

	CheckTaskAllocation();
	InitKeyManagers();

	Super::BeginPlay();
}

void UEasyAITaskManager::PreloadTaskLists(const TArray<TSoftObjectPtr<UEasyAITaskList>>& InLists)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	TArray<FSoftObjectPath> AssetPaths;

	for (const TSoftObjectPtr<UEasyAITaskList>& TaskListSoftPtr : InLists)
	{
		AssetPaths.Add(TaskListSoftPtr.ToSoftObjectPath());
	}

	StreamableHandle = StreamableManager.RequestAsyncLoad(AssetPaths, FStreamableDelegate::CreateUObject(this, &UEasyAITaskManager::OnAllTaskListsLoaded));
}

void UEasyAITaskManager::OnAllTaskListsLoaded()
{
	for (const TSoftObjectPtr<UEasyAITaskList>& TaskListSoftPtr : TaskLists)
	{
		UEasyAITaskList* TaskListPtr = TaskListSoftPtr.Get();
		if (IsValid(TaskListPtr))
		{
			TaskListPointers.Add(TaskListPtr);

			for (UClass* TaskClass : TaskListPtr->TaskClasses)
			{
				if (TaskClass) AllTaskClasses.Add(TaskClass);
			}

			for (UClass* ForcedTaskClass : TaskListPtr->ForcedTaskClasses)
			{
				if (ForcedTaskClass) AllTaskClasses.Add(ForcedTaskClass);
			}
		}
	}

	TaskPreloadState = ETaskPreloadState::Preloaded;
}

void UEasyAITaskManager::SetAllocationTimer()
{
	GetWorld()->GetTimerManager().SetTimer(CheckTaskAllocationTimer, this, &UEasyAITaskManager::CheckTaskAllocation, 0.1f, false);
}

void UEasyAITaskManager::CheckTaskAllocation()
{
	if (AIOwner == nullptr || AIOwnerPawn == nullptr)
	{
		SetAllocationTimer();
		return;
	}

	switch (TaskPreloadState)
	{
	case ETaskPreloadState::NoPreload:
		TaskPreloadState = ETaskPreloadState::Complete;
		break;

	case ETaskPreloadState::Pending:
		SetAllocationTimer();
		break;

	case ETaskPreloadState::Preloaded:
		if (AllocateTasks())
		{
			TaskPreloadState = ETaskPreloadState::Complete;
		}
		break;

	case ETaskPreloadState::Complete:
		//No action required
		break;

	default:
		break;
	}
}

void UEasyAITaskManager::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (!AllTasks.IsEmpty())
	{
		for (UEasyAITask* TaskObject : AllTasks)
		{
			if (IsValid(TaskObject))
			{
				TaskObject->OnDestroyed();
			}
		}
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

bool UEasyAITaskManager::CanValidatePawnForAiOwner()
{
	//Get the Pawn from the owning AI Controller.
	if (AIOwner)
	{
		if (!AIOwner->GetPawn())
		{
			return false;
		}
		else
		{
			if (AIOwnerPawn)
			{
				return true;
			}
			else
			{
				AIOwnerPawn = AIOwner->GetPawn();
				return true;
			}
		}
	}
	//Return true if attached to pawn
	else
	{
		if (AIOwnerPawn)
		{
			return true;
		}
		else
		{
			AIOwnerPawn = Cast<APawn>(GetOwner());
			return true;
		}
	}
}

void UEasyAITaskManager::ResetPawnForAiOwner()
{
	//Reset the Pawn

	AIOwnerPawn = nullptr;

	if (CanValidatePawnForAiOwner())
	{
		//Also for Tasks
		for (UEasyAITask* TaskObject : AllTasks)
		{
			if (IsValid(TaskObject))
			{
				TaskObject->AIOwnerPawn = AIOwnerPawn;
			}
		}
	}
	else
	{
		UE_LOG(LogEasyAI, Warning, TEXT("Task Manager : Reset AI Owner Pawn failed. Call this after possess!"));
	}
}

void UEasyAITaskManager::InitKeyManagers()
{
	//Local
	if (!LocalKeys)
	{
		UEasyAIKeyStorage* NewKeys = NewObject<UEasyAIKeyStorage>(this, UEasyAIKeyStorage::StaticClass());
		LocalKeys = NewKeys;
	}

	//Shared
	UEasyAISubsystem* AiSub = UEasyAISubsystem::GetCurrent(GetWorld());
	if (AiSub)
	{
		SharedKeys = AiSub->GetSharedKeys(GetClass());
		if (!SharedKeys)
		{
			SharedKeys = AiSub->CreateNewSharedKeys(GetClass());
		}
	}
}

bool UEasyAITaskManager::ShouldUpdateTask() const
{
	return GetWorld()->TimeSeconds - TaskManagerUpdateTime >= TaskUpdateRate;
}

void UEasyAITaskManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
	{
		return;
	}

	//The pawn is assigned when possessed, so we cannot just set it in BeginPlay and need to set and validate it here.
	if (!CanValidatePawnForAiOwner())
	{
		return;
	}

	if (IsActive() && TaskPreloadState == ETaskPreloadState::Complete)
	{
		for (UEasyAITask* TaskObject : AllTasks)
		{
			if (IsValid(TaskObject))
			{
				//Get real score from previous frame, so it can be read across all tasks alike no matter in which order they are updated.
				TaskObject->ActualScore = TaskObject->GetScore();

				//Tick when active
				if (TaskObject->IsActive())
				{
					TaskObject->ReceiveTick(DeltaTime);
				}

				//Set default score
				if (ShouldUpdateTask())
				{
					TaskObject->TaskDefaultUpdate();
				}
			}
		}

		UpdateTasks();
	}
	
}

void UEasyAITaskManager::UpdateTasks()
{
	//Not updating every frame
	if (ShouldUpdateTask())
	{
		AutoRunUtilityTasks();

		//This is where run utility task has to be called
		OnUpdateTasks();

		//Select task for every channel
		for (const FGameplayTag& TaskChannel : AllChannels)
		{
			SelectTask(TaskChannel);
		}

		TaskManagerUpdateTime = GetWorld()->TimeSeconds;
	}
}

void UEasyAITaskManager::AutoRunUtilityTasks()
{
	if (!TaskLists.IsEmpty())
	{
		for (const UEasyAITaskList* List : TaskListPointers)
		{
			if (List && List->bAutoRunUtilityTask)
			{
				for (UClass* TaskClass : List->TaskClasses)
				{
					RunUtilityTask(TaskClass);
				}
			}
		}
	}
}

void UEasyAITaskManager::SelectTask(const FGameplayTag& Channel)
{
	if (!HasAnyTasks() || !Channel.IsValid())
	{
		return;
	}

	//Check if we have an active task that has to be finished first
	const UEasyAITask* ActiveTask = GetActiveTask(Channel);
	if (ActiveTask)
	{
		//Check if we can override with the higher scoring one, or if we need to wait until the current task is finished
		if (ActiveTask->IsExplicitFinish() || ActiveTask->IsForced() || ActiveTask->IsBelowMinActiveTime())
		{
			return;
		}
	}

	//Group Scoring Tasks by Channel
	TArray<UEasyAITask*> ScoringTasks;
	for (UEasyAITask* TaskObject : AllTasks)
	{
		if (TaskObject->IsValidForScoring(Channel))
		{
			ScoringTasks.Add(TaskObject);
		}
	}

	if (ScoringTasks.Num() <= 0)
	{
		return;
	}

	//Here we can begin the scoring process
	bool bHasEquals = false;
	UEasyAITask* HighestScoring = CompareScores(ScoringTasks, bHasEquals);
	if (!HighestScoring)
	{
		return;
	}

	if (HighestScoring->IsActive())
	{
		return;
	}

	//Handle Finish
	if (ActiveTask)
	{
		//Favor active until one scores higher or it finishes, to prevent flipping around between equal tasks.
		if (bHasEquals)
		{
			return;
		}

		//Finish Task with failsave, so we never have a case where multiple tasks per channel are active
		uint16 ActiveCount = 0;
		for (auto It = ActiveTasks.CreateIterator(); It; ++It)
		{
			UEasyAITask* Task = *It;
			if (!Task)
			{
				continue;
			}

			if (Task->Channel == Channel)
			{
				ActiveCount++;
				Task->InternalFinishTask(false);
				LastFinishedTasks.Add(Task->Channel, Task);
				It.RemoveCurrent();
			}
		}

		//Should never happen
		if (ActiveCount > 1)
		{
			UE_LOG(LogEasyAI, Warning, TEXT("Multiple Tasks were finished for Channel: %s !  Last Finished Task may be inaccurate!"), *Channel.ToString());
		}
	}

	//Get the last finished task and proceed.
	UEasyAITask* LastFinished = GetLastFinishedTask(Channel);
	if (LastFinished)
	{
		HighestScoring->SetTaskActive(LastFinished);
		LastFinishedTasks.Remove(LastFinished->Channel);
	}
	else
	{
		HighestScoring->SetTaskActive(nullptr);
	}
}

UEasyAITask* UEasyAITaskManager::CompareScores(const TArray<UEasyAITask*>& Scorers, bool& bHasEquals) const
{
	if (Scorers.Num() <= 0)
	{
		return nullptr;
	}

	if (Scorers.Num() == 1)
	{
		return Scorers[0];
	}

	TArray<UEasyAITask*> SortedTasks = Scorers;
	SortedTasks.Sort([&](const UEasyAITask& A, const UEasyAITask& B)
	{
		return A.GetWeightedScore() > (B.GetWeightedScore() + MinOverrideScore);
	});

	//Check for equal scores
	bHasEquals = false;
	TArray<UEasyAITask*> BestTasks;

	BestTasks.Reserve(SortedTasks.Num());
	BestTasks.Add(SortedTasks[0]);
	const float BestScore = SortedTasks[0]->GetWeightedScore();

	for (int32 i = 1; i < SortedTasks.Num(); ++i)
	{
		if (!FMath::IsNearlyEqual(BestScore, SortedTasks[i]->GetWeightedScore()))
		{
			break;
		}

		bHasEquals = true;
		BestTasks.Add(SortedTasks[i]);
	}

	//Select from equals
	if (bHasEquals)
	{
		//Tie breaker by weight
		Algo::RandomShuffle(BestTasks);
		BestTasks.Sort([&](const UEasyAITask& A, const UEasyAITask& B)
		{
			return A.Weight > B.Weight;
		});

		//With shuffle and unstable sort, this will return a random element in case of equals
		return BestTasks[0];
	}

	return SortedTasks[0];
}

void UEasyAITaskManager::RunUtilityTask(TSubclassOf<UEasyAIUtilityTask> TaskClass)
{
	UEasyAITask* NewTask = GetTask(TaskClass);

	if (!NewTask)
	{
		NewTask = SpawnNewTask(TaskClass);
		return;
	}

	if (NewTask)
	{
		UEasyAIUtilityTask* UtilityTask = Cast<UEasyAIUtilityTask>(NewTask);
		if (UtilityTask)
		{
			UtilityTask->UpdateTask();
		}
	}
}

void UEasyAITaskManager::FinishActiveTask(FGameplayTag Channel)
{
	if (!Channel.IsValid())
	{
		Channel = DefaultChannelTag;
	}

	UEasyAITask* CurrentTask = GetActiveTask(Channel);

	if (CurrentTask)
	{
		CurrentTask->FinishTask();
	}
}

bool UEasyAITaskManager::ForceTask(TSubclassOf<UEasyAIForcedTask> TaskClass)
{
	UEasyAITask* NewTask = GetTask(TaskClass);
	if (!NewTask)
	{
		NewTask = SpawnNewTask(TaskClass);
		if (!NewTask)
		{
			return false;
		}
	}

	UEasyAITask* PreviousTask = GetActiveTask(TaskClass.GetDefaultObject()->Channel);
	if (PreviousTask)
	{
		NewTask->SetTaskActive(PreviousTask);
		PreviousTask->FinishTask();
	}
	else
	{
		NewTask->SetTaskActive(nullptr);
	}

	return true;
}

bool UEasyAITaskManager::IsChannelActive(FGameplayTag Channel) const
{
	const UEasyAITask* CurrentTask = GetActiveTask(Channel);

	if (CurrentTask && CurrentTask->bIgnoreChannelActive)
	{
		return false;
	}

	return CurrentTask != nullptr;
}

UEasyAITask* UEasyAITaskManager::GetTask(TSubclassOf<UEasyAITask> TaskClass) const
{
	if (!HasAnyTasks())
	{
		return nullptr;
	}

	for (UEasyAITask* TaskObject : AllTasks)
	{
		if (IsValid(TaskObject))
		{
			if (TaskObject->GetClass() == TaskClass)
			{
				return TaskObject;
			}
		}
	}

	return nullptr;
}

UEasyAITask* UEasyAITaskManager::GetActiveTask(FGameplayTag Channel) const
{
	if (!Channel.IsValid())
	{
		Channel = DefaultChannelTag;
	}

	for (UEasyAITask* ActiveTask : ActiveTasks)
	{
		if (ActiveTask->Channel == Channel)
		{
			return ActiveTask;
		}
	}

	return nullptr;
}

void UEasyAITaskManager::AddActiveTask(UEasyAITask* TheTask)
{
	ActiveTasks.Add(TheTask);
}

void UEasyAITaskManager::RemoveActiveTask(UEasyAITask* TheTask)
{
	LastFinishedTasks.Add(TheTask->Channel, TheTask);
	ActiveTasks.Remove(TheTask);
}

UEasyAITask* UEasyAITaskManager::GetLastFinishedTask(const FGameplayTag& Channel) const
{
	if (!Channel.IsValid())
	{
		return nullptr;
	}

	return LastFinishedTasks.FindRef(Channel);
}

UEasyAITask* UEasyAITaskManager::SpawnNewTask(const TSubclassOf<UEasyAITask>& TaskClass)
{
	if (TaskClass.GetDefaultObject())
	{
		UEasyAITask* NewTask = NewObject<UEasyAITask>(GetOwner(), TaskClass);
		if (NewTask)
		{
			NewTask->InitTask(this);

			AllTasks.Add(NewTask);

			//Add the new tag. If invalid the task will always use the default channel.
			AllChannels.Add(NewTask->Channel);

			return NewTask;
		}
	}

	return nullptr;
}

bool UEasyAITaskManager::AllocateTasks()
{
	bool bAllocated = false;

	for (const TSubclassOf<UEasyAITask>& TaskClass : AllTaskClasses)
	{
		SpawnNewTask(TaskClass);
	}

	bAllocated = AllTasks.Num() == AllTaskClasses.Num();

	if (!bAllocated)
	{
		UE_LOG(LogEasyAI, Error, TEXT("Precache Task Allocation failed"));
		return false;
	}

	return bAllocated;
}


