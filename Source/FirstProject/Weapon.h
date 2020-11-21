// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/BoxComponent.h"


#include "Weapon.generated.h"

UENUM()
enum class EWeaponState
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equip UMETA(DisplayName = "Equip"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};
/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category="SaveData")
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item | Particles")
	bool bUseWeaponParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	USoundCue* OnEquipSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	USoundCue* OnAttackSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Weapon")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	AController* WeaponInstigator;

	FORCEINLINE void SetWeaponInstigator(AController* WeaponInst) { WeaponInstigator = WeaponInst; }

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
    virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EnableCollision();
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void DisableCollision();
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void PlaySound();
	

	void Equip(class AMainCharacter* Character);

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }

protected:
	virtual void BeginPlay() override;
};
