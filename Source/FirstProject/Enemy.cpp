// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"



#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Serialization/BufferArchive.h"

#define OUT

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Shpere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(600.0f);
	
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat Shpere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.0f);

	CombatCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Combat Collider"));
	// CombatCollider->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("CombatSocket")));
	CombatCollider->SetupAttachment(GetMesh(), FName(TEXT("CombatSocket")));
	CombatCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	bIsInCombatRange = false;
	StartingLocation = FVector(0.0f);
	AggroWaitTimeSeconds = 5.0f;
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 10.0f;
	bIsAttacking = false;

	AttackMaxTimeSeconds = 2.0f;
	AttackMinTimeSeconds = 0.5f;

	InterpSpeed = 20.0f;
	bInterpToPlayer = false;
	DestroyDelayTimeSeconds = 3.0f;
	bInitialized = false;
}

TArray<uint8> AEnemy::CaptureActorState()
{
	FBufferArchive SaveData;
	
	FVector ActorLocation = GetActorLocation();
	FRotator ActorRotation = GetActorRotation();
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveData << LevelName;
	SaveData << Health;
	SaveData << MaxHealth;
	SaveData << ActorLocation;
	SaveData << ActorRotation;
	
	return SaveData;
}

void AEnemy::RestoreActorState(TArray<uint8> State, bool bIsLevelChanging)
{
	if (bIsLevelChanging) { return; }
	if (!bInitialized)
	{
		Initialize();
	}
	FMemoryReader RestoreData = FMemoryReader(State, true);
	RestoreData.Seek(0);

	FVector ActorLocation;
	FRotator ActorRotation;
	FString LevelName;

	RestoreData << LevelName;
	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (LevelName == CurrentLevel)
	{
		RestoreData << Health;
		RestoreData << MaxHealth;
		RestoreData << ActorLocation;
		RestoreData << ActorRotation;

		SetActorLocation(ActorLocation);
		SetActorRotation(ActorRotation);
		if (Health <= 0)
		{
			Die();
		}
	}
	RestoreData.FlushCache();
	RestoreData.Close();
}

void AEnemy::EnableCollision()
{
	CombatCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DisableCollision()
{
	CombatCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::PlaySound()
{
	if (OnAttackSound)
	{
		UGameplayStatics::PlaySound2D(this, OnAttackSound);
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::Disappear, DestroyDelayTimeSeconds);
}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw{0.0f, LookAtRotation.Yaw, 0.0f};
	return LookAtRotationYaw;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}

void AEnemy::Initialize()
{
	if (bInitialized) { return; }

	AIController = Cast<AAIController>(GetController());
	StartingLocation = GetActorLocation();
	Health = MaxHealth;

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AggroOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	CombatCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackOnOverlapBegin);
	CombatCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackOnOverlapEnd);
	CombatCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	bInitialized = true;
}


// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Dead || !IsAlive()) { return; }
	if(NavPath)
	{
		// UKismetSystemLibrary::DrawDebugSphere(this, NavPath->GetDestinationLocation(), 25.0f, 16, FLinearColor::Red, 0.1f, 1.0f);
		if (GetActorLocation().Equals(NavPath->GetDestinationLocation(), GetCapsuleComponent()->GetUnscaledCapsuleRadius() + 10.0f))
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
	}
	else if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Idle)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	}

	if (bInterpToPlayer && CombatTarget && EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AggroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			GetWorldTimerManager().ClearTimer(AggroWaitTimer);
			if (MainCharacter->IsAlive())
			{
				MoveToTarget(MainCharacter);
			}
		}
	}
}

void AEnemy::AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			FVector Location = NavPath->GetDestinationLocation();
			MoveToLocation(Location);
			GetWorldTimerManager().SetTimer(AggroWaitTimer, this, &AEnemy::MoveToStart, AggroWaitTimeSeconds);
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter && MainCharacter->IsAlive())
		{
			MainCharacter->SetCombatTarget(this);
			if (MainCharacter->PlayerController)
			{
				MainCharacter->PlayerController->ShowEnemyHealthBar();
			}
			CombatTarget = MainCharacter;
			bIsInCombatRange = true;
			Attack();
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			bIsInCombatRange = false;
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->UpdateCombatTarget();
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				CombatTarget = nullptr;
				if (MainCharacter->IsAlive())
				{
					MoveToTarget(MainCharacter);
				}
			}
		}
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	if(!AIController || !IsAlive()) { return; }

	AIController->StopMovement();
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToActor);
	
	FAIMoveRequest MoveRequest(Target);
	MoveRequest.SetAcceptanceRadius(5.0f);

	AIController->MoveTo(MoveRequest, OUT &NavPath);

	// auto PathPoints = NavPath->GetPathPoints();
	// for (const auto Point : PathPoints)
	// {
	// 	UKismetSystemLibrary::DrawDebugSphere(this, Point.Location, 25.0f, 16, FLinearColor::Red, 10.0f, 1.0f);
	// }
}

void AEnemy::MoveToStart()
{
	MoveToLocation(StartingLocation);
}


void AEnemy::MoveToLocation(FVector Target)
{
	if(!AIController || !IsAlive()) { return; }

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
	FAIMoveRequest MoveRequest(Target);
	MoveRequest.SetAcceptanceRadius(.1f);

	AIController->MoveTo(MoveRequest, OUT &NavPath);
}

void AEnemy::Disappear()
{
	// Destroy();
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AEnemy::Attack()
{
	if (bIsAttacking || !IsAlive()) { return; }
	if (AIController)
	{
		AIController->StopMovement();
	}
	
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	bIsAttacking = true;
	SetInterpToPlayer(true);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(TEXT("Attack_1"), CombatMontage);
	}
}

void AEnemy::AttackEnd()
{
	if (!IsAlive()) { return; }
	bIsAttacking = false;
	SetInterpToPlayer(false);
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	if (CombatTarget && !CombatTarget->IsAlive())
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::MoveToStart, AggroWaitTimeSeconds);
	}
	else if (bIsInCombatRange && CombatTarget && CombatTarget->IsAlive())
	{
		float AttackWaitTime = FMath::FRandRange(AttackMinTimeSeconds, AttackMaxTimeSeconds);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackWaitTime);
	}
	else if (CombatTarget && CombatTarget->IsAlive())
	{
		MoveToTarget(CombatTarget);
	}
}

bool AEnemy::IsAlive()
{
	return EnemyMovementStatus != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::AttackOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && IsAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter && MainCharacter->IsAlive())
		{
			if (MainCharacter->HitParticles)
			{
				const USkeletalMeshSocket* ParticleSocket = GetMesh()->GetSocketByName(TEXT("ParticleSocket"));
				if (ParticleSocket)
				{
					FVector SocketLocation = ParticleSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->HitParticles, SocketLocation, FRotator(0.0f), false);
				}
			}
			if (MainCharacter->OnHitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainCharacter->OnHitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainCharacter, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::AttackOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Health > DamageAmount)
	{
		Health -= DamageAmount;
	}
	else
	{
		Health = 0.0f;
		Die();
	}
	return DamageAmount;
}

void AEnemy::Die()
{
	if (!IsAlive()) { return; }
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Death")));
	}
	
	if (CombatTarget)
	{
		CombatTarget->UpdateCombatTarget();
	}

	GetWorldTimerManager().ClearTimer(AttackTimer);
	CombatCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



