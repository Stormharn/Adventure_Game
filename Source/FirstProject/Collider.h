// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "ColliderMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"

#include "Collider.generated.h"

UCLASS()
class FIRSTPROJECT_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	UColliderMovementComponent* MyMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;
	
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	FORCEINLINE void SetStaticMeshComponent(UStaticMeshComponent* Mesh) { StaticMeshComponent = Mesh; }
	
	FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
	
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* Camera) {CameraComponent = Camera; }
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* SpringArm) { SpringArmComponent = SpringArm; }

private:

	void MoveForward(float Value);
	void MoveRight(float Value);
	void PitchCamera(float Value);
	void YawCamera(float Value);

	FVector2D CameraInput;
};
