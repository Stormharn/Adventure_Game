// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Hazard.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AHazard : public AItem
{
	GENERATED_BODY()

public:
	AHazard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float Damage;

    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
    virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
};
