// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"


#include "MainCharacter.h"
#include "Components/BillboardComponent.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Level Transition Volume"));
	SetRootComponent(TransitionVolume);
	TransitionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TransitionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TransitionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(GetRootComponent());

	TransitionLevelName = TEXT("Default");
}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TransitionLevelName == TEXT("Default"))
	{
		UE_LOG(LogTemp, Error, TEXT("Level Transition Name is not set."))
		return;
	}
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (MainCharacter)
	{
		MainCharacter->SwitchLevel(TransitionLevelName);
	}
}

