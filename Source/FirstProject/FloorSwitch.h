// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Components")
	UBoxComponent* TriggerBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Components")
	UStaticMeshComponent* FloorSwitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Components")
	UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Floor Switch")
	FVector InitialDoorLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Floor Switch")
	FVector InitialSwitchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch")
	float DoorCloseDelay;

	FTimerHandle SwitchTimer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void RaiseDoor();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void RaiseFloorSwitch();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category= "Floor Switch")
	void UpdateDoorLocation(float ZHeight);

	UFUNCTION(BlueprintCallable, Category= "Floor Switch")
	void UpdateFloorSwitchLocation(float ZHeight);

	void CloseDoor();
};
