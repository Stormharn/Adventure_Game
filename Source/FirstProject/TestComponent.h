// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Saveable.h"
#include "Components/ActorComponent.h"
#include "TestComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPROJECT_API UTestComponent : public UActorComponent, public ISaveable
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere)
	int32 RandomNumber;

	virtual TArray<uint8> CaptureState() override;
	virtual void RestoreState(TArray<uint8> State, bool bIsLevelChanging) override;
		
};
