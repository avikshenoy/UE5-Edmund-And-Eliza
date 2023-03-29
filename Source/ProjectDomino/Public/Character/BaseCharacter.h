// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class PROJECTDOMINO_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	
	ABaseCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

protected:
	
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */
	
	/** Combat */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual void StrongAttack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	virtual bool CanAttack(const FName& TypeOfAttack);
	void DisableCapsule();
	bool IsAlive();
	void DisableMeshCollision();
	virtual void SpawnDefaultWeapon();

	/** Animation Montage */
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage(UAnimMontage* Montage);
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage(const FName& SectionName);
	virtual void PlayPlayerAttackMontage(UAnimMontage* Montage, const FName& SectionName);
	virtual void PlayQuickTurnMontage(UAnimMontage* Montage);
	virtual int32 PlayChaseActionMontage();
	virtual int32 PlayBeginChaseMontage();
	virtual void PlayIntimidationMontage();
	virtual int32 PlayPatrolPointReachedMontage();
	void StopMontage(UAnimMontage* Montage);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	virtual void QuickTurnEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LightDamage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StrongDamage;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* StrongAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* QuickTurnMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* CrouchQuickTurnMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ChaseActionMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* BeginChaseMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* IntimidationMontage;

private:

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* PatrolPointReachedMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> ChaseActionMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> BeginChaseMontageSections;

	UPROPERTY(EditAnywhere, Category = Patrol)
	TArray<FName> PatrolPointReachedMontageSections;

public:	
	
	/** Getters */
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

};
