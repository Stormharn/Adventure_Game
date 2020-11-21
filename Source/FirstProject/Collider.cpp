// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	// SphereComponent->SetupAttachment(GetRootComponent());
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if(MeshComponentAsset.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		StaticMeshComponent->SetWorldScale3D(FVector(0.8f));
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetSpringArmComponent(), USpringArmComponent::SocketName);

	MyMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("Movement Component"));
	MyMovementComponent->UpdatedComponent = GetRootComponent();

	CameraInput = FVector2D(0.0f);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArmComponent->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.0f, -15.0f);
	SpringArmComponent->SetWorldRotation(NewSpringArmRotation);


}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return MyMovementComponent;
}

void ACollider::MoveForward(float Value)
{
	FVector Forward = GetActorForwardVector();
	if (MyMovementComponent)
	{
		MyMovementComponent->AddInputVector(Forward * Value);
	}
}

void ACollider::MoveRight(float Value)
{
	FVector Right = GetActorRightVector();
	if (MyMovementComponent)
	{
		MyMovementComponent->AddInputVector(Right * Value);
	}
}

void ACollider::PitchCamera(float Value)
{
	CameraInput.Y = Value;
}

void ACollider::YawCamera(float Value)
{
	CameraInput.X = Value;
}

