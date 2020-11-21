// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SavingSystem.generated.h"

UCLASS()
class FIRSTPROJECT_API USavingSystem : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	USavingSystem();

	static bool SaveGame(const UWorld* World, const FString& SaveName);
	static bool LoadGame(const UWorld* World, const FString& SaveName, const bool& bIsLevelChangeing);
	static bool SaveGameExists(const FString& SaveName);

private:
	static void CaptureEntityState(const UWorld* World, TMap<FString, TArray<uint8>>& DataMap);
	static void CaptureComponentsState(AActor* Actor, TMap<FString, TArray<uint8>>& DataMap);	
	static void RestoreEntityState(const UWorld* World, const TMap<FString, TArray<uint8>>& DataMap, const bool& bIsLevelChanging);
	static void RestoreComponentsState(const AActor* Actor, const TMap<FString, TArray<uint8>>& DataMap, const bool& bIsLevelChanging);
};