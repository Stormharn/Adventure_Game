// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "AIController.h"
#include "MainCharacter.h"
#include "Saveable.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"


#include "Enemy.generated.h"

UENUM()
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "Move To Target"),
	EMS_MoveToActor		UMETA(DisplayName = "Move To Actor"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Dead			UMETA(DisplayName = "Dead"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AEnemy : public ACharacter, public ISaveableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual TArray<uint8> CaptureActorState() override;
	virtual void RestoreActorState(TArray<uint8> State, bool bIsLevelChanging) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	USphereComponent* AggroSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	USphereComponent* CombatSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UBoxComponent* CombatCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	AAIController* AIController;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
	AMainCharacter* CombatTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	bool bIsInCombatRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FVector StartingLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AggroWaitTimeSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	USoundCue* OnHitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	USoundCue* OnAttackSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	UAnimMontage* CombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Other")
	bool bIsAttacking;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	float DestroyDelayTimeSeconds;

	UFUNCTION(BlueprintCallable, Category="Other")
	void EnableCollision();
	UFUNCTION(BlueprintCallable, Category="Other")
    void DisableCollision();
	UFUNCTION(BlueprintCallable, Category="Other")
    void PlaySound();
	UFUNCTION(BlueprintCallable, Category="Other")
    void DeathEnd();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	float AttackMinTimeSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	float AttackMaxTimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Other")
	TSubclassOf<UDamageType> DamageTypeClass;

	float InterpSpeed;
	bool bInterpToPlayer;
	FORCEINLINE void SetInterpToPlayer(bool Interp) { bInterpToPlayer = Interp; }

	FRotator GetLookAtRotationYaw(FVector Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	UFUNCTION()
	virtual void AggroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
    void AttackOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
    void AttackOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Die();

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AMainCharacter* Target);
	void MoveToStart();
	void MoveToLocation(FVector Target);
	void Disappear();

	void Attack();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	bool IsAlive();

private:
	
	FNavPathSharedPtr NavPath{};
	FTimerHandle AggroWaitTimer;
	FTimerHandle AttackTimer;
	FTimerHandle DestroyTimer;
	bool bInitialized;

	void Initialize();
};
