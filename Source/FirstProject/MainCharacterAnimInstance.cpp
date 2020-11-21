// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
    if(!Pawn)
    {
        Pawn = TryGetPawnOwner();
        if (Pawn)
        {
            MainCharacter = Cast<AMainCharacter>(Pawn);
        }
    }

    MovementSpeed = 0.0f;
    bIsInAir = false;
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
    if(!Pawn)
    {
        Pawn = TryGetPawnOwner();
    }

    if (!Pawn) { return; }

    const FVector Speed = Pawn->GetVelocity();
    const FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
    MovementSpeed = LateralSpeed.Size();

    bIsInAir = Pawn->GetMovementComponent()->IsFalling();

    if (!MainCharacter)
    {
        MainCharacter = Cast<AMainCharacter>(Pawn);
    }
}
