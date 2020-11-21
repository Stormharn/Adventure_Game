// Fill out your copyright notice in the Description page of Project Settings.


#include "TestComponent.h"

#include "Serialization/BufferArchive.h"

// Sets default values for this component's properties
UTestComponent::UTestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	RandomNumber = 0;
}


// Called when the game starts
void UTestComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	RandomNumber = FMath::RandRange(1, 1000);
}

TArray<uint8> UTestComponent::CaptureState()
{
	FBufferArchive SaveData;
	SaveData << RandomNumber;
	return SaveData;
}

void UTestComponent::RestoreState(TArray<uint8> State, bool bIsLevelChanging)
{
	FMemoryReader RestoreData = FMemoryReader(State, true);
	RestoreData.Seek(0);

	RestoreData << RandomNumber;

	RestoreData.FlushCache();
	RestoreData.Close();
}


