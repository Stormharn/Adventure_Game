// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"



#include "Enemy.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(GetRootComponent());
    bUseWeaponParticles = false;
    WeaponState = EWeaponState::EWS_Pickup;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Combat Box Collider"));
    CollisionBox->SetupAttachment(GetRootComponent());
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    Damage = 25.0f;
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
    {
        AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
        if (MainCharacter)
        {
            MainCharacter->SetActiveOverlappedItem(this);
        }
    }
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    if (OtherActor)
    {
        AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
        if (MainCharacter)
        {
            MainCharacter->SetActiveOverlappedItem(nullptr);
        }
    }
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor)
    {
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if (Enemy)
        {
            if (Enemy->HitParticles)
            {
                const USkeletalMeshSocket* ParticleSocket = SkeletalMesh->GetSocketByName(TEXT("ParticleSocket"));
                if (ParticleSocket)
                {
                    FVector SocketLocation = ParticleSocket->GetSocketLocation(SkeletalMesh);
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.0f), false);
                }
            }
            if (Enemy->OnHitSound)
            {
                UGameplayStatics::PlaySound2D(this, Enemy->OnHitSound);
            }
            if (DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
            }
        }
    }
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::EnableCollision()
{
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DisableCollision()
{
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlaySound()
{
    if(OnAttackSound)
    {
        UGameplayStatics::PlaySound2D(this, OnAttackSound);
    }
}

void AWeapon::Equip(AMainCharacter* Character)
{
    if(!Character) { return; }

    SetWeaponInstigator(Character->GetController());

    SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    SkeletalMesh->SetSimulatePhysics(false);

    const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName(TEXT("RightHandSocket"));
    if(RightHandSocket)
    {
        bShouldRotate = false;
        RightHandSocket->AttachActor(this, Character->GetMesh());
    }
    if (OnEquipSound)
    {
        UGameplayStatics::PlaySound2D(this, OnEquipSound);
    }
    Character->SetEquippedWeapon(this);
    Character->SetActiveOverlappedItem(nullptr);
    if (!bUseWeaponParticles)
    {
        IdleParticles->Deactivate();
    }
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}
