// Easy Utility AI - (C) 2025 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "AIUtilityData.h"
#include "UObject/NoExportTypes.h"
#include "EasyAIKeyStorage.generated.h"

UCLASS(Blueprintable, BlueprintType)
class EASYUTILITYAI_API UEasyAIKeyStorage : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Transient)
	TMap<FGameplayTag, uint8> BoolMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, float> FloatMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, int> IntMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, uint8> ByteMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FVector> VectorMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FRotator> RotatorMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FName> NameMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FString> StringMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UClass*> ClassMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UObject*> ObjectMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, AActor*> ActorMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FGameplayTag> TagMap;

public:

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	bool GetBoolKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	int GetIntKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	float GetFloatKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	uint8 GetByteKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	FVector GetVectorKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	FRotator GetRotatorKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	FName GetNameKey(FGameplayTag Key) const;
	
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	FString GetStringKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	UClass* GetClassKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	UObject* GetObjectKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	AActor* GetActorKey(FGameplayTag Key) const;

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys")
	FGameplayTag GetTagKey(FGameplayTag Key) const;

public:

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetBoolKey(FGameplayTag Key, bool Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetIntKey(FGameplayTag Key, int Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetFloatKey(FGameplayTag Key, float Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetByteKey(FGameplayTag Key, uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetVectorKey(FGameplayTag Key, FVector Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetRotatorKey(FGameplayTag Key, FRotator Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetNameKey(FGameplayTag Key, FName Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetStringKey(FGameplayTag Key, FString Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetClassKey(FGameplayTag Key, UClass* Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetObjectKey(FGameplayTag Key, UObject* Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetActorKey(FGameplayTag Key, AActor* Value);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys")
	void SetTagKey(FGameplayTag Key, FGameplayTag Value);
};

