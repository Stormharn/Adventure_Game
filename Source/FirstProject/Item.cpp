// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"



#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Volume"));
	SetRootComponent(CollisionVolume);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	IdleParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Idle Particles"));
	IdleParticles->SetupAttachment(GetRootComponent());

	bShouldRotate = false;
	RotationRate = 45.0f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldRotate)
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += DeltaTime * RotationRate;

		SetActorRotation(NewRotation);
	}

}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!Cast<AMainCharacter>(OtherActor)) { return; }

	if(OverlapParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation());
	}
	if(OverlapSound)
	{
		UGameplayStatics::PlaySound2D(this, OverlapSound);
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

