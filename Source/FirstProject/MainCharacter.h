// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ItemStorage.h"
#include "MainPlayerController.h"
#include "PickupTypes.h"
#include "SaveableActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "UObject/ObjectMacros.h"

#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMin UMETA(DisplayName = "Below Min"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_Recovering UMETA(DisplayName = "Recovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMainCharacter : public ACharacter, public ISaveableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	int32 Coins;

	UPROPERTY(EditDefaultsOnly, Category="SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	AMainPlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Variables)
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Variables)
	EStaminaStatus StaminaStatus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Variables);
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Variables);
	float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Variables)
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Variables)
	float SprintingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Variables)
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Variables)
	float StaminaWarnLevel;

	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	class AEnemy* CombatTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	FVector CombatTargetLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items")
	class AWeapon* EquippedWeapon;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items")
	class AItem* ActiveOverlappedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHasCombatTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	UAnimMontage* CombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	USoundCue* OnHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Variables")
	TSubclassOf<AEnemy> EnemyFilter;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool HasCombatTarget() const;
	UFUNCTION(BlueprintCallable)
	void DeathEnd();
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquipWeapon(Weapon); }
	FORCEINLINE void SetActiveOverlappedItem(AItem* Item) { ActiveOverlappedItem = Item; }
	FORCEINLINE bool IsAlive() const { return MovementStatus != EMovementStatus::EMS_Dead; }

	void ReduceHealth(float AmountToReduce);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void IncreaseHealth(float AmountToIncrease);
	void IncreasePickupItem(EPickupTypes Type, int32 Count);
	void SetMovementStatus(EMovementStatus Status);
	virtual void Jump() override;
	void UpdateCombatTarget();

	bool SwitchLevel(FName LevelName);
	// UFUNCTION(BlueprintCallable)
	// void SaveGame();
	// UFUNCTION(BlueprintCallable)
	// void LoadGame(bool bSetPosition);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Variables)
	AItemStorage* ItemStorage;

	virtual TArray<uint8> CaptureActorState() override;
	virtual void RestoreActorState(TArray<uint8> State, bool bIsLevelChanging) override;
	
private:
	bool bShiftKeyDown;
	bool bLMBDown;
	bool bESCDown;
	bool bIsMovingForward;
	bool bIsMovingRight;
	bool bInitialized;

	void Initialize();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	bool CanMove(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Die();
	void IncreaseCoins(int32 Count);
	void ShiftKeyDown();
	void ShiftKeyUp();
	void Attack();
	void LMBDown();
	void LMBUp();
	void ESCDown();
	void ESCUp();
	void EquipWeapon(AWeapon* Weapon);
	
};
