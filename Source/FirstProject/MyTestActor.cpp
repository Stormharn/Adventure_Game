// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTestActor.h"

// Sets default values
AMyTestActor::AMyTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(MeshComponent);
	MeshComponent->OnBeginCursorOver.AddDynamic(this, &AMyTestActor::OnBeginMouseOver);
}

void AMyTestActor::OnBeginMouseOver(UPrimitiveComponent* ComponentMouseIsOver)
{
	UE_LOG(LogTemp, Warning, TEXT("The Mouse Over Is Working!!"));
}

void AMyTestActor::OnEndMouseOver(UPrimitiveComponent* ComponentMouseIsOver)
{
	UE_LOG(LogTemp, Warning, TEXT("The Mouse Over Exit Is Working!!"));
}

// Called when the game starts or when spawned
void AMyTestActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnEndCursorOver.AddDynamic(this, &AMyTestActor::OnEndMouseOver);
}

// Called every frame
void AMyTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

