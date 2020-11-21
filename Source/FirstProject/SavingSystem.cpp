// Fill out your copyright notice in the Description page of Project Settings.


#include "SavingSystem.h"

#include "Saveable.h"
#include "SaveableActor.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/BufferArchive.h"

// Sets default values
USavingSystem::USavingSystem()
{
}

bool USavingSystem::SaveGame(const UWorld* World, const FString& SaveName)
{
	TMap<FString, TArray<uint8>> EntityDataMap;
	FBufferArchive BinaryEntityDataMap;

	CaptureEntityState(World, EntityDataMap);
	BinaryEntityDataMap << EntityDataMap;

	if (BinaryEntityDataMap.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Data Found To Save!"));
		BinaryEntityDataMap.FlushCache();
		BinaryEntityDataMap.Empty();
		return false;
	}

	bool bSaveSuccess;
	if(FFileHelper::SaveArrayToFile(BinaryEntityDataMap, *FString::Printf(TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), *SaveName)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Succeded!"));
		bSaveSuccess = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Failed!"));
		bSaveSuccess = false;
	}
	BinaryEntityDataMap.FlushCache();
	BinaryEntityDataMap.Empty();

	return bSaveSuccess;
}

void USavingSystem::CaptureEntityState(const UWorld* World, TMap<FString, TArray<uint8>>& DataMap)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(World, USaveableActor::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		ISaveableActor* SaveableActor = Cast<ISaveableActor>(Actor);
		FString ActorKey;
		if (SaveableActor->Guid != TEXT(""))
		{
			ActorKey = SaveableActor->Guid.ToString();
		}
		else
		{
			ActorKey = Actor->GetName();
		}
		FString Addon = TEXT("_ActorSave");
		FString ActorSaveKey = ActorKey + Addon;
		
		TArray<uint8> ActorState = SaveableActor->CaptureActorState();
		if (ActorState.Num() > 0)
		{
			DataMap.Add(ActorSaveKey, ActorState);
		}

		TMap<FString, TArray<uint8>> ComponentDataMap;
		FBufferArchive BinaryComponentDataMap;

		CaptureComponentsState(Actor, ComponentDataMap);
		BinaryComponentDataMap << ComponentDataMap;
		
		if (BinaryComponentDataMap.Num() <= 0)
		{
			BinaryComponentDataMap.FlushCache();
			BinaryComponentDataMap.Empty();
			continue;
		}

		DataMap.Add(ActorKey, BinaryComponentDataMap);
		BinaryComponentDataMap.FlushCache();
		BinaryComponentDataMap.Empty();
	}
}

void USavingSystem::CaptureComponentsState(AActor* Actor, TMap<FString, TArray<uint8>>& DataMap)
{
	TArray<UActorComponent*> FoundComponents = Actor->GetComponentsByInterface(USaveable::StaticClass());
	
	for (auto Component : FoundComponents)
	{
		ISaveable* SaveableComponent = Cast<ISaveable>(Component);
		if (SaveableComponent)
		{
			DataMap.Add(Component->GetName(), SaveableComponent->CaptureState());
		}
	}
}

bool USavingSystem::LoadGame(const UWorld* World, const FString& SaveName, const bool& bIsLevelChangeing)
{
	TArray<uint8> BinaryFromDisk;
	if(!FFileHelper::LoadFileToArray(BinaryFromDisk, *FString::Printf(TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), *SaveName)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Load From File Failed!"));
		return false;
	}
	if (BinaryFromDisk.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Binary Data Found To Load!"));
		BinaryFromDisk.Empty();
		return false;
	}

	TMap<FString, TArray<uint8>> EntityDataMap;
	FMemoryReader BinaryEntityData = FMemoryReader(BinaryFromDisk, true);
	BinaryEntityData.Seek(0);
	BinaryEntityData << EntityDataMap;
	if (EntityDataMap.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Map Data Found To Load!"));
		BinaryEntityData.FlushCache();
		BinaryFromDisk.Empty();
		BinaryEntityData.Close();
		return false;
	}

	RestoreEntityState(World, EntityDataMap, bIsLevelChangeing);
	
	BinaryEntityData.FlushCache();
	BinaryFromDisk.Empty();
	BinaryEntityData.Close();

	UE_LOG(LogTemp, Warning, TEXT("Load Succeded!"));
	return true;
}

bool USavingSystem::SaveGameExists(const FString& SaveName)
{
	TArray<uint8> DataOnDisk;
	if(!FFileHelper::LoadFileToArray(DataOnDisk, *FString::Printf(TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), *SaveName)))
	{
		UE_LOG(LogTemp, Error, TEXT("Save File Not Found!"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Save File Found!"));
	DataOnDisk.Empty();
	return true;
}

void USavingSystem::RestoreEntityState(const UWorld* World, const TMap<FString, TArray<uint8>>& DataMap, const bool& bIsLevelChanging)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(World, USaveableActor::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		ISaveableActor* SaveableActor = Cast<ISaveableActor>(Actor);
		FString ActorKey;
		if (SaveableActor->Guid != TEXT(""))
		{
			ActorKey = SaveableActor->Guid.ToString();
		}
		else
		{
			ActorKey = Actor->GetName();
		}
		FString Addon = TEXT("_ActorSave");
		FString ActorSaveKey = ActorKey + Addon;
		
		if (DataMap.Contains(ActorSaveKey))
		{
			SaveableActor->RestoreActorState(DataMap[ActorSaveKey], bIsLevelChanging);
		}
		
		if (DataMap.Contains(ActorKey))
		{
			FMemoryReader BinaryComponentMap = FMemoryReader(DataMap[ActorKey], true);
			TMap<FString, TArray<uint8>> ComponentDataMap;
			BinaryComponentMap << ComponentDataMap;
			if (ComponentDataMap.Num() <= 0)
			{
				BinaryComponentMap.FlushCache();
				BinaryComponentMap.Close();
				continue;
			}

			RestoreComponentsState(Actor, ComponentDataMap, bIsLevelChanging);

			BinaryComponentMap.FlushCache();
			BinaryComponentMap.Close();
		}
	}
}

void USavingSystem::RestoreComponentsState(const AActor* Actor, const TMap<FString, TArray<uint8>>& DataMap, const bool& bIsLevelChanging)
{
	TArray<UActorComponent*> FoundComponents = Actor->GetComponentsByInterface(USaveable::StaticClass());

	for (auto Component : FoundComponents)
	{
		if (DataMap.Contains(Component->GetName()))
		{
			ISaveable* SaveableComponent = Cast<ISaveable>(Component);
			if (SaveableComponent)
			{
				SaveableComponent->RestoreState(DataMap[Component->GetName()], bIsLevelChanging);
			}
		}
	}
}
