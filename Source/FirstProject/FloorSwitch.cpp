// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetBoxExtent(FVector(62.0f, 62.0f, 32.0f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Swithc"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	DoorCloseDelay = 3.0f;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	RaiseDoor();
	LowerFloorSwitch();
	GetWorldTimerManager().ClearTimer(SwitchTimer);
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
	GetWorldTimerManager().SetTimer(SwitchTimer, this, &AFloorSwitch::CloseDoor, DoorCloseDelay);
}

void AFloorSwitch::UpdateDoorLocation(float ZHeight)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += ZHeight;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float ZHeight)
{
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += ZHeight;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	LowerDoor();
	RaiseFloorSwitch();
}


