// Easy Utility AI - (C) 2025 by Michael Hegemann


#include "EasyAITask.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIUtilityFunctions.h"

//
//Easy AI Task 
//

UWorld* UEasyAITask::GetWorld() const
{
	return World;
}

void UEasyAITask::InitTask(UEasyAITaskManager* TaskManager)
{
	//Always use default channel if invalid
	if (!Channel.IsValid())
	{
		Channel = DefaultChannelTag;
	}

	Manager = TaskManager;

	if (Manager)
	{
		World = Manager->GetWorld();

		//Try find the owner
		if (Manager->AIOwner)
		{
			AIOwner = Manager->AIOwner;
		}

		if (Manager->AIOwnerPawn)
		{
			AIOwnerPawn = Manager->AIOwnerPawn;
		}

		OnBeginPlay();
	}
}

void UEasyAITask::SetTaskActive(UEasyAITask* Previous)
{
	if (!Manager)
	{
		return;
	}

	if (Previous && Previous != this)
	{
		Previous->ConsecutiveCount = 0;
	}

	Manager->AddActiveTask(this);

	bWasActiveOnce = true;
	bIsActive = true;
	ElapsedTime = 0.0f;
	OverallCount++;
	ConsecutiveCount++;
	CooldownTime = 0.0f;

	OnStart(Previous);
}

void UEasyAITask::InternalFinishTask(const bool bRemoveActiveTask)
{
	if (!GetWorld())
	{
		return;
	}

	ElapsedTime = 0.0f;
	FinishTimeStamp = GetWorld()->GetTimeSeconds();
	bIsActive = false;

	if (Manager && bRemoveActiveTask)
	{
		Manager->RemoveActiveTask(this);
	}

	OnFinish();
}

void UEasyAITask::FinishTask()
{
	InternalFinishTask(true);
}

void UEasyAITask::ReceiveTick(float DeltaTime)
{
	if (!Manager)
	{
		return;
	}

	if (!IsActive())
	{
		return;
	}

	LastFinishTime = 0;
	ElapsedTime += DeltaTime;
	OnTick(DeltaTime);
}

UEasyAITask* UEasyAITask::GetTask(TSubclassOf<UEasyAITask> TaskClass) const
{
	if (Manager)
	{
		return Manager->GetTask(TaskClass);
	}

	return nullptr;
}

UEasyAITask* UEasyAITask::GetActiveTask() const
{
	if (Manager)
	{
		return Manager->GetActiveTask(Channel);
	}

	return nullptr;
}

bool UEasyAITask::IsChannelActive(FGameplayTag ActiveChannel) const
{
	if (Manager)
	{
		return Manager->IsChannelActive(ActiveChannel);
	}

	return false;
}

float UEasyAITask::GetTaskScore(TSubclassOf<UEasyAIUtilityTask> TaskClass, bool bWeighted) const
{
	if (UEasyAITask* Task = UEasyAITask::GetTask(TaskClass))
	{
		return bWeighted ? Task->GetWeightedScore(true) : Task->ActualScore;
	}

	return bWeighted ? GetWeightedScore(true) : ActualScore;
}

float UEasyAITask::GetWeightedScore(const bool bCached) const
{
	const float InScore = bCached ? ActualScore : Score;

	if (FMath::IsNearlyEqual(Weight, 1.f))
	{
		return InScore;
	}

	//Return score scaled by weight logarithmically (diminishing returns).
	const float LogWeight = FMath::Log2(Weight + 1.f); 
	const float FinalScore = FMath::Max(0.f, InScore * LogWeight);

	return FMath::RoundToFloat(FinalScore * SCORE_ROUND) / SCORE_ROUND;
}

//
//Easy AI Utility Task 
//

void UEasyAIUtilityTask::NormalizeScore()
{
	Score = FMath::Clamp(Score, 0.0f, 1.0f);
	Score = FMath::RoundToFloat(Score * SCORE_ROUND) / SCORE_ROUND;
}

void UEasyAIUtilityTask::SetCooldown(float Time)
{
	CooldownTime = Time;
}

void UEasyAIUtilityTask::UpdateCooldown()
{
	if (!GetWorld())
	{
		return;
	}

	LastFinishTime = GetWorld()->GetTimeSeconds() - FinishTimeStamp;

	//Auto finish . We call this here since this function is always called regardless of branch.
	if (MaxActiveTime > 0 && ElapsedTime >= MaxActiveTime)
	{
		FinishTask();
	}
}

void UEasyAIUtilityTask::SetScore(float NewScore)
{
	Score = NewScore;
	NormalizeScore();
}

void UEasyAIUtilityTask::SetUtilityScore(TArray<FUtilityScore> Scorers)
{
	const float NewScore = UAIUtilityFunctions::CreateUtilityScore(Scorers, ScoringType);
	SetScore(NewScore);
}

void UEasyAIUtilityTask::TaskDefaultUpdate()
{
	//This can happen with spawned AI using precached Tasks
	if (!AIOwnerPawn)
	{
		AIOwnerPawn = Manager->AIOwnerPawn;
	}

	SetScore(0);
	UpdateCooldown();

	bIsUpdating = false;
}

void UEasyAIUtilityTask::UpdateTask()
{
	if (IsDelayed())
	{
		SetScore(0);
		return;
	}
	else
	{
		SetScore(DefaultScore);
	}

	OnScore();

	if (Score > 0)
	{
		NormalizeScore();
	}
	else
	{
		Score = 0;
	}

	bIsUpdating = true;
}

