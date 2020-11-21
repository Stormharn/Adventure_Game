// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMeshComponent);

	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);
	InterpSpeed = 4.0f;
	InterpPause = 2.0f;
	bInterping = false;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	GetWorldTimerManager().SetTimer(InterpHandle, this, &AFloatingPlatform::ToggleInterping, InterpPause);
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);

		SetActorLocation(Interp);
		if ((Interp - CurrentLocation).IsNearlyZero(.1f))
		{
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpHandle, this, &AFloatingPlatform::ToggleInterping, InterpPause);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& Vec1, FVector& Vec2)
{
	FVector Temp = Vec1;
	Vec1 = Vec2;
	Vec2 = Temp;
}

