// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawning Box"));
	SetRootComponent(SpawningVolume);
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	// if (ActorToSpawn_1 && ActorToSpawn_2 && ActorToSpawn_3 && ActorToSpawn_4)
	// {
	// 	SpawnArray.Add(ActorToSpawn_1);
	// 	SpawnArray.Add(ActorToSpawn_2);
	// 	SpawnArray.Add(ActorToSpawn_3);
	// 	SpawnArray.Add(ActorToSpawn_4);
	// }
	if (ActorToSpawn_1)
	{
		SpawnArray.Add(ActorToSpawn_1);
	}
	if (ActorToSpawn_2)
	{
		SpawnArray.Add(ActorToSpawn_2);
	}
	if (ActorToSpawn_3)
	{
		SpawnArray.Add(ActorToSpawn_3);
	}
	if (ActorToSpawn_4)
	{
		SpawnArray.Add(ActorToSpawn_4);
	}
	if (ActorToSpawn_5)
	{
		SpawnArray.Add(ActorToSpawn_5);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint() const
{
	FVector Extent = SpawningVolume->GetScaledBoxExtent();
	FVector Origin = SpawningVolume->GetComponentLocation();

	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor() const
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num()-1);
		return SpawnArray[Selection];
	}

	return nullptr;
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ClassToSpawn, const FVector& SpawnLocation)
{
	if (ClassToSpawn)
	{
		UWorld* World = GetWorld();
		if(World)
		{
			FActorSpawnParameters SpawnParams;
			AEnemy* Enemy = Cast<AEnemy>(World->SpawnActor<AActor>(ClassToSpawn, SpawnLocation, FRotator(0.0f, -90.0f, 0.0f), SpawnParams));
			if (Enemy)
			{
				Enemy->SpawnDefaultController();
				AAIController* AIContoller = Cast<AAIController>(Enemy->GetController());
				if (AIContoller)
				{
					Enemy->AIController = AIContoller;
				}
			}
		}
	}
}

