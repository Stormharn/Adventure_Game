// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    if(!Pawn)
    {
        Pawn = TryGetPawnOwner();
        if (Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }

    MovementSpeed = 0.0f;
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
    if(!Pawn)
    {
        Pawn = TryGetPawnOwner();
    }

    if (!Pawn) { return; }
    
    const FVector Speed = Pawn->GetVelocity();
    const FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
    MovementSpeed = LateralSpeed.Size();

    if (!Enemy)
    {
        Enemy = Cast<AEnemy>(Pawn);
    }
}
