// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard.h"

#include "MainCharacter.h"

AHazard::AHazard()
{
    Damage = 15.0f;
}

void AHazard::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if(OtherActor)
    {
        AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
        if (Player)
        {
            Player->ReduceHealth(Damage);
            Destroy();
        }
    }
}

void AHazard::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}
