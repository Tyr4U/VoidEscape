// Easy Utility AI - (C) 2025 by Michael Hegemann


#include "EasyAIKeyStorage.h"
#include "EasyAITaskManager.h"

//Getters

bool UEasyAIKeyStorage::GetBoolKey(FGameplayTag Key) const
{
	const uint8* Result = BoolMap.Find(Key);
	if (Result)
	{
		return *Result == 1;
	}

	return false;
}

int UEasyAIKeyStorage::GetIntKey(FGameplayTag Key) const
{
	const int* Result = IntMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

float UEasyAIKeyStorage::GetFloatKey(FGameplayTag Key) const
{
	const float* Result = FloatMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

uint8 UEasyAIKeyStorage::GetByteKey(FGameplayTag Key) const
{
	const uint8* Result = ByteMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

FVector UEasyAIKeyStorage::GetVectorKey(FGameplayTag Key) const
{
	const FVector* Result = VectorMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FVector::ZeroVector;
}

FRotator UEasyAIKeyStorage::GetRotatorKey(FGameplayTag Key) const
{
	const FRotator* Result = RotatorMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FRotator::ZeroRotator;
}

FName UEasyAIKeyStorage::GetNameKey(FGameplayTag Key) const
{
	const FName* Result = NameMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FName();
}

FString UEasyAIKeyStorage::GetStringKey(FGameplayTag Key) const
{
	const FString* Result = StringMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FString();
}

UClass* UEasyAIKeyStorage::GetClassKey(FGameplayTag Key) const
{
	return ClassMap.FindRef(Key);
}

UObject* UEasyAIKeyStorage::GetObjectKey(FGameplayTag Key) const
{
	return ObjectMap.FindRef(Key);
}

AActor* UEasyAIKeyStorage::GetActorKey(FGameplayTag Key) const
{
	return ActorMap.FindRef(Key);
}

FGameplayTag UEasyAIKeyStorage::GetTagKey(FGameplayTag Key) const
{
	return TagMap.FindRef(Key);
}

//Setters

void UEasyAIKeyStorage::SetBoolKey(FGameplayTag Key, bool Value)
{
	BoolMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetIntKey(FGameplayTag Key, int Value)
{
	IntMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetFloatKey(FGameplayTag Key, float Value)
{
	FloatMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetByteKey(FGameplayTag Key, uint8 Value)
{
	ByteMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetVectorKey(FGameplayTag Key, FVector Value)
{
	VectorMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetRotatorKey(FGameplayTag Key, FRotator Value)
{
	RotatorMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetNameKey(FGameplayTag Key, FName Value)
{
	NameMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetStringKey(FGameplayTag Key, FString Value)
{
	StringMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetClassKey(FGameplayTag Key, UClass* Value)
{
	ClassMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetObjectKey(FGameplayTag Key, UObject* Value)
{
	ObjectMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetActorKey(FGameplayTag Key, AActor* Value)
{
	ActorMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetTagKey(FGameplayTag Key, FGameplayTag Value)
{
	TagMap.Add(Key, Value);
}
