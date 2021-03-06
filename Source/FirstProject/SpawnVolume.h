// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Critter.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class FIRSTPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Spawning")
	UBoxComponent* SpawningVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spawning")
	TSubclassOf<AActor> ActorToSpawn_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spawning")
	TSubclassOf<AActor> ActorToSpawn_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spawning")
	TSubclassOf<AActor> ActorToSpawn_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spawning")
	TSubclassOf<AActor> ActorToSpawn_4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spawning")
	TSubclassOf<AActor> ActorToSpawn_5;

	TArray<TSubclassOf<AActor>> SpawnArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawning")
	UParticleSystem* SpawnParticles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category= "Spawning")
	FVector GetSpawnPoint() const;
	UFUNCTION(BlueprintPure, Category= "Spawning")
	TSubclassOf<AActor> GetSpawnActor() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Spawning")
	void SpawnOurActor(UClass* ClassToSpawn, const FVector& SpawnLocation);
};
