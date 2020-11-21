// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

UColliderMovementComponent::UColliderMovementComponent()
{
    MaxSpeed = 100.0f;
}

void UColliderMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * MaxSpeed;
    

    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult HitResult;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, HitResult);

        if(HitResult.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMovementThisFrame, 1.0f - HitResult.Time, HitResult.Normal, HitResult);
        }
    }
    
}
