// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UBehaviorTree;
class AEnemyController;
class UPawnSensingComponent;
class AWeapon;
class APlayerCharacter;

UCLASS()
class PROJECTDOMINO_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();

	/** <AActor */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */


protected:

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack(const FName& TypeOfAttack) override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void SpawnDefaultWeapon() override;
	virtual void PlayDodgeMontage(const FName& SectionName) override;
	/** </ABaseCharacter> */


	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	/** AI Behavior*/
	UFUNCTION(BlueprintCallable)
	void InitializeEnemy();
	UFUNCTION(BlueprintCallable)
	void InitializePlayerPointer();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void StartChaseActionTimer();
	void PerformChaseAction();
	void StartChaseSpeedTimer();
	void StartChaseStrafeTimer();
	void StartSecondChaseStrafeTimer();
	void StartThirdChaseStrafeTimer();
	void StartChaseDodgeTimer();
	void PerformChaseDodge();
	UFUNCTION(BlueprintCallable)
	void PerformEvade();
	void PerformChaseSpeedChange();
	void PerformChaseStrafe();
	void PerformSecondChaseStrafe();
	void PerformThirdChaseStrafe();
	void StartChaseOrStrafeTimer();
	void ChaseOrStrafeDecision();
	void StartFirstAttackStrafeTimer();
	void PerformAttackStrafe();
	void StartNextAttackStrafeTimer();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsInsideAttackStrafeRadius();
	bool IsPatrolling();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	void ClearChaseActionTimer();
	void ClearChaseDodgeTimer();
	void ClearChaseSpeedTimer();
	void ClearChaseStrafeTimer();
	void ClearSecondChaseStrafeTimer();
	void ClearThirdChaseStrafeTimer();
	void ClearChaseOrStrafeTimer();
	void ClearAttackStrafeTimer();
	void SetMovementAttributes(bool IsStrafing, bool OrientMovement, bool UseDesiredRot, float Speed);
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	FVector GetStrafeLocation();
	void MoveToLocation(FVector Location);
	void FocusOnTarget();
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UFUNCTION(BlueprintCallable)
	void StartChasingTarget();

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	double CombatRadius;

	UPROPERTY(EditAnywhere)
	double AttackRadius;

	UPROPERTY(EditAnywhere)
	double AttackStrafeRadius;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double StrafeAcceptanceRadius;

	UPROPERTY()
	AEnemyController* EnemyController;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DodgeMontageSections;

	UPROPERTY(EditAnywhere)
	double PatrolRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeedWalk;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeedRun;

	FTimerHandle AttackTimer;

	FTimerHandle ChaseActionTimer;

	FTimerHandle ChaseSpeedTimer;

	FTimerHandle ChaseStrafeTimer;

	FTimerHandle ChaseOrStrafeTimer;

	FTimerHandle SecondChaseStrafeTimer;

	FTimerHandle ThirdChaseStrafeTimer;

	FTimerHandle AttackStrafeTimer;

	FTimerHandle DodgeTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseActionDecisionMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseActionDecisionMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseSpeedMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseSpeedMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseStrafeMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseStrafeMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float NextChaseStrafeMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float NextChaseStrafeMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseOrStrafeMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseOrStrafeMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackStrafeMin;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackStrafeMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FirstAttackStrafeTime;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DodgeMin;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DodgeMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 EvadeChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsStrafing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StrafingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bIsRunning;

	/** Old Behavior tree stuff, might use, might not use */
private:

	UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	//UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	//FVector PatrolPoint;

public:

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
