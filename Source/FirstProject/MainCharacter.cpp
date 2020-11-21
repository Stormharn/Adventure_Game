// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "Enemy.h"
#include "FirstSaveGame.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(GetCameraBoom(), USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetCapsuleSize(24.0f, 70.0f);

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.0f;
	MaxStamina = 100.0f;
	Health = 100.0f;
	Stamina = 100.0f;
	Coins = 0;
	StaminaDrainRate = 5.0f;
	StaminaWarnLevel = 25.0f;
	RunningSpeed = 250.0f;
	SprintingSpeed = 500.0f;

	bShiftKeyDown = false;
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	bLMBDown = false;
	bAttacking = false;
	bESCDown = false;

	InterpSpeed = 15.0f;
	bInterpToEnemy = false;
	bHasCombatTarget = false;
	bIsMovingForward = false;
	bIsMovingRight = false;
	bInitialized = false;
	Guid = FName(TEXT("MainCharacter_Player01"));
	
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;	
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::Jump()
{
	if (!CanMove(1.0f)) { return; }
	Super::Jump();
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors{};
	GetOverlappingActors(OUT OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) { return; }

	FVector ActorLoction = GetActorLocation();
	float MinDistance{MAX_flt};
	AEnemy* ClosestEnemy{nullptr};
	for (auto Actor : OverlappingActors)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (Enemy && Enemy->IsAlive())
		{
			float CurDistance = FVector::Distance(ActorLoction, Enemy->GetActorLocation());
			if (CurDistance < MinDistance)
			{
				MinDistance = CurDistance;
				ClosestEnemy = Enemy;
			}
		}
	}
	if (ClosestEnemy)
	{
		SetCombatTarget(ClosestEnemy);
		if (PlayerController)
		{
			PlayerController->ShowEnemyHealthBar();
		}
		bHasCombatTarget = true;
	}
	else
	{
		SetCombatTarget(nullptr);
		if (PlayerController)
		{
			PlayerController->HideEnemyHealthBar();
		}
		bHasCombatTarget = false;
	}
}

bool AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevelName = World->GetMapName();
		CurrentLevelName.RemoveFromStart(World->StreamingLevelsPrefix);
		
		if (FName(CurrentLevelName) != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
			return true;
		}
	}
	return false;
}

// void AMainCharacter::SaveGame()
// {
// 	// UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
// 	// SaveGameInstance->CharacterStats.Health = Health;
// 	// SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
// 	// SaveGameInstance->CharacterStats.Stamina = Stamina;
// 	// SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
// 	// SaveGameInstance->CharacterStats.Coins = Coins;
// 	// SaveGameInstance->CharacterStats.Location = GetActorLocation();
// 	// SaveGameInstance->CharacterStats.Rotation = GetActorRotation();
// 	//
// 	// UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
// 	USavingSystem::SaveGame(GetWorld(), TEXT("TestSave2"));
// }
//
// void AMainCharacter::LoadGame(bool bSetPosition)
// {
// 	// UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
// 	// LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
// 	//
// 	// Health = LoadGameInstance->CharacterStats.Health;
// 	// MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
// 	// Stamina = LoadGameInstance->CharacterStats.Stamina;
// 	// MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
// 	// Coins = LoadGameInstance->CharacterStats.Coins;
// 	//
// 	// if (bSetPosition)
// 	// {
// 	// 	SetActorLocation(LoadGameInstance->CharacterStats.Location);
// 	// 	SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
// 	// }
// 	USavingSystem::LoadGame(GetWorld(), TEXT("TestSave2"));
// }

TArray<uint8> AMainCharacter::CaptureActorState()
{
	FBufferArchive SaveData;
	
	FVector ActorLocation = GetActorLocation();
	FRotator ActorRotation = GetActorRotation();
	FString EquippedWeaponName = TEXT("");
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (EquippedWeapon)
	{
		EquippedWeaponName = EquippedWeapon->WeaponName;
	}

	SaveData << LevelName;
	SaveData << Health;
	SaveData << MaxHealth;
	SaveData << Stamina;
	SaveData << MaxStamina;
	SaveData << Coins;
	SaveData << ActorLocation;
	SaveData << ActorRotation;
	SaveData << EquippedWeaponName;
	
	return SaveData;
}

void AMainCharacter::RestoreActorState(TArray<uint8> State, bool bIsLevelChanging)
{
	if (!bInitialized)
	{
		Initialize();
	}
	FMemoryReader RestoreData = FMemoryReader(State, true);
	RestoreData.Seek(0);

	FVector ActorLocation;
	FRotator ActorRotation;
	FString EquippedWeaponName;
	FString LevelName;
	
	RestoreData << LevelName;
	RestoreData << Health;
	RestoreData << MaxHealth;
	RestoreData << Stamina;
	RestoreData << MaxStamina;
	RestoreData << Coins;
	RestoreData << ActorLocation;
	RestoreData << ActorRotation;
	RestoreData << EquippedWeaponName;
	RestoreData.FlushCache();
	RestoreData.Close();
	
	if (bIsLevelChanging && LevelName != TEXT(""))
	{
		if (!SwitchLevel(FName(LevelName)))
		{
			SetActorLocation(ActorLocation);
			SetActorRotation(ActorRotation);
		}
	}
	else
	{
		FString CurrentLevel = GetWorld()->GetMapName();
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
		if (LevelName == CurrentLevel)
		{
			SetActorLocation(ActorLocation);
			SetActorRotation(ActorRotation);
		}
	}
	
	if(EquippedWeaponName != TEXT("") && WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons && Weapons->WeaponMap.Contains(EquippedWeaponName))
		{
			AWeapon* EquipWeapon = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[EquippedWeaponName]);
			if (EquipWeapon)
			{
				EquipWeapon->Equip(this);
			}
		}
	}

	if (Health <= 0.0f)
	{
		Health = 0.0f;
		Die();
	}
	else
	{
		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;	
	}
}

void AMainCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMainCharacter::Attack()
{
	if (bAttacking || !IsAlive()) { return; }
	bAttacking = true;
	if (CombatTarget)
	{
		SetInterpToEnemy(true);
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		int32 Section = FMath::RandRange(0, 1);
		switch (Section)
		{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName(TEXT("Attack_1")), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 2.0f);
				AnimInstance->Montage_JumpToSection(FName(TEXT("Attack_2")), CombatMontage);
				break;
			default: ;//Do Nothing
		}
	}
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;
	if (!IsAlive() || !CanMove(1.0f)) { return; }
	if (ActiveOverlappedItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappedItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappedItem(nullptr);
		}
	}
	else if (EquippedWeapon && !bAttacking)
	{
		Attack();
	}
}

void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMainCharacter::ESCDown()
{
	bESCDown = true;
	if (PlayerController)
	{
		PlayerController->TogglePauseMenu();
	}
}

void AMainCharacter::ESCUp()
{
	bESCDown = false;
}

void AMainCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = Weapon;
}

void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw{0.0f, LookAtRotation.Yaw, 0.0f};
	return LookAtRotationYaw;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}

void AMainCharacter::Initialize()
{
	if (bInitialized) { return; }
	
	Health = MaxHealth;
	Stamina = MaxStamina;
	Coins = 0;
	PlayerController = Cast<AMainPlayerController>(GetController());
	bInitialized = true;
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) { return; }
	float DeltaStamina = StaminaDrainRate * DeltaTime;
	switch (StaminaStatus)
	{
		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown && (bIsMovingForward || bIsMovingRight))
			{
				if (Stamina - DeltaStamina <= StaminaWarnLevel)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMin);
				}
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				if (Stamina + DeltaStamina > MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_BelowMin:
			if (bShiftKeyDown)
			{
				if (Stamina - DeltaStamina <= 0.0f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0.0f;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
			}
			else
			{
				if (Stamina + DeltaStamina >= StaminaWarnLevel)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown)
			{
				Stamina = 0.0f;
			}
			else
			{
				SetStaminaStatus(EStaminaStatus::ESS_Recovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		case EStaminaStatus::ESS_Recovering:
			if (Stamina + DeltaStamina >= StaminaWarnLevel)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			Stamina += DeltaStamina;
			MovementStatus = EMovementStatus::EMS_Normal;
			break;
		default:
			;//Do Nothing
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (PlayerController)
		{
			PlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnByRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpByRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::ESCUp);

}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

bool AMainCharacter::HasCombatTarget() const
{
	if (CombatTarget)
	{
		return true;
	}
	return false;
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	if (PlayerController)
	{
		PlayerController->ShowGameOver(false);
	}
}

void AMainCharacter::ReduceHealth(float AmountToReduce)
{
	if (Health > AmountToReduce)
	{
		Health -= AmountToReduce;
	}
	else
	{
		Health = 0.0f;
		Die();
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ReduceHealth(DamageAmount);
	return DamageAmount;
}

void AMainCharacter::IncreaseHealth(float AmountToIncrease)
{
	if (Health + AmountToIncrease > MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += AmountToIncrease;
	}
}

void AMainCharacter::IncreasePickupItem(EPickupTypes Type, int32 Count)
{
	if (Type == EPickupTypes::MAX) { return; }
	// if(Type == EPickupTypes::Coins)
	// {
	// 	IncreaseCoins(Count);
	// }
	switch (Type)
	{
		case EPickupTypes::Coins:
			IncreaseCoins(Count);
			break;
		case EPickupTypes::HealthPotion:
			IncreaseHealth(Count);
			break;
		default: ; //Do Nothing
	}
}

void AMainCharacter::MoveForward(float Value)
{
	bIsMovingForward = false;
	if (CanMove(Value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		bIsMovingForward = true;
	}
}

void AMainCharacter::MoveRight(float Value)
{
	bIsMovingRight = false;
	if (CanMove(Value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		bIsMovingRight = true;
	}
}

void AMainCharacter::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}
void AMainCharacter::LookUp(float Value)
{
	if (CanMove(Value))
	{	
		AddControllerPitchInput(Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	if (CanMove(Rate))
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	if (CanMove(Rate))
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

bool AMainCharacter::CanMove(float Value)
{
	if (PlayerController)
	{
		return Controller && Value != 0.0f && !bAttacking && IsAlive() && !PlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMainCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Death")));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainCharacter::IncreaseCoins(int32 Count)
{
	Coins += Count;
	if (Coins >= 25 && PlayerController)
	{
		PlayerController->ShowGameOver(true);
	}
}

