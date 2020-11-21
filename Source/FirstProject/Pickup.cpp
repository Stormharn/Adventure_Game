// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "MainCharacter.h"

APickup::APickup()
{
    Count = 1;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if(OtherActor)
    {
        AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
        if (Player)
        {
            Player->IncreasePickupItem(Type, Count);
            Destroy();
        }
    }
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}
