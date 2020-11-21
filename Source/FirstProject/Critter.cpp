// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SetRootComponent(SphereComponent);
	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 300.0f));
	CameraComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	// AutoPossessPlayer = EAutoReceiveInput::Player0;
	CurrentVelocity = FVector(0.0f);
	MaxSpeed = 100.0f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
}

void ACritter::MoveForward(float Value)
{
	CurrentVelocity.X = FMath::Clamp(Value, -1.0f, 1.0f) * MaxSpeed;
}

void ACritter::MoveRight(float Value)
{
	CurrentVelocity.Y = FMath::Clamp(Value, -1.0f, 1.0f) * MaxSpeed;
}

