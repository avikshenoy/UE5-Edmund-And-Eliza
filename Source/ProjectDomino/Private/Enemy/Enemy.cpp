// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "DrawDebugHelpers.h"

AEnemy::AEnemy() :
	AcceptanceRadius(75.f),
	StrafeAcceptanceRadius(50.f),
	CombatRadius(1000.f),
	AttackRadius(150.f),
	AttackStrafeRadius(150.f),
	PatrolRadius(200.f),
	PatrolWaitMin(3.f),
	PatrolWaitMax(8.f),
	PatrollingSpeed(125.f),
	ChasingSpeedWalk(150.f),
	ChasingSpeedRun(300.f),
	StrafingSpeed(100.f),
	AttackMin(0.5f),
	AttackMax(1.f),
	DeathLifeSpan(8.f),
	ChaseActionDecisionMin(5.f),
	ChaseActionDecisionMax(8.f),
	ChaseSpeedMin(8.f),
	ChaseSpeedMax(12.f),
	ChaseStrafeMin(10.f),
	ChaseStrafeMax(15.f),
	NextChaseStrafeMin(3.f),
	NextChaseStrafeMax(5.f),
	AttackStrafeMin(2.f),
	AttackStrafeMax(3.f),
	DodgeMin(8.f),
	DodgeMax(14.f),
	EvadeChance(3),
	bIsRunning(false)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 5500.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

	if (bIsStrafing && CombatTarget)
	{
		FocusOnTarget();
	}

	/** Debug Tools */
	//UE_LOG(LogTemp, Log, TEXT("Enemy Enum is %s"), *UEnum::GetValueAsString(EnemyState));

	//UE_LOG(LogTemp, Warning, TEXT("bIsStrafing is %s"), bIsStrafing ? TEXT("true") : TEXT("false"));
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (!IsDead())
	{
		ShowHealthBar();
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	ClearChaseActionTimer();
	ClearChaseSpeedTimer();
	ClearChaseStrafeTimer();
	ClearSecondChaseStrafeTimer();
	ClearThirdChaseStrafeTimer();
	ClearChaseOrStrafeTimer();
	ClearAttackStrafeTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopMontage(AttackMontage);

	if (IsInsideAttackRadius())
	{
		if (!IsDead())
		{
			StartAttackTimer();
		}
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnemy();
	InitializePlayerPointer();

	// Old behavior tree stuff

	/*const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);*/

	//if (EnemyController)
	//{
	//	EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);

	//	EnemyController->RunBehaviorTree(BehaviorTree);
	//}
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::EES_Dead;

	ClearAttackTimer();
	ClearChaseActionTimer();
	ClearChaseSpeedTimer();
	ClearChaseStrafeTimer();
	ClearSecondChaseStrafeTimer();
	ClearThirdChaseStrafeTimer();
	ClearChaseOrStrafeTimer();
	ClearAttackStrafeTimer();
	ClearChaseDodgeTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	SetMovementAttributes(false, false, false, 0.f);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	CombatTarget = nullptr;
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;

	EnemyState = EEnemyState::EES_Engaged;
	PlayGruntSound(GetActorLocation());
	PlayAttackMontage(AttackMontage);
}

bool AEnemy::CanAttack(const FName& TypeOfAttack)
{
	if (TypeOfAttack.GetStringLength() > 0) {}

	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();

	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();

	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::PlayDodgeMontage(const FName& SectionName)
{
	PlayRandomMontageSection(DodgeMontage, DodgeMontageSections);
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AEnemyController>(GetController());
	SetMovementAttributes(false, true, false, PatrollingSpeed);
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializePlayerPointer()
{
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		SetMovementAttributes(false, true, false, PatrollingSpeed);

		PatrolTarget = ChoosePatrolTarget();

		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
		
		PlayPatrolSound(GetActorLocation());
		PlayPatrolPointReachedMontage();
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		ClearChaseActionTimer();
		ClearChaseSpeedTimer();
		ClearChaseStrafeTimer();
		ClearSecondChaseStrafeTimer();
		ClearThirdChaseStrafeTimer();
		ClearChaseOrStrafeTimer();
		ClearAttackStrafeTimer();
		ClearChaseDodgeTimer();

		LoseInterest();

		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();

		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if (CanAttack(FName("EnemyAttack")))
	{
		ClearChaseActionTimer();
		ClearChaseSpeedTimer();
		ClearChaseStrafeTimer();
		ClearSecondChaseStrafeTimer();
		ClearThirdChaseStrafeTimer();
		ClearChaseOrStrafeTimer();
		ClearAttackStrafeTimer();

		StartAttackTimer();
		if (IsInsideAttackStrafeRadius())
		bIsRunning = false;
		StartFirstAttackStrafeTimer();

	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	SetMovementAttributes(false, true, false, PatrollingSpeed);
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	if (IsPatrolling())
	{
		if (PlayerCharacter->GetCharacterMovement()->IsCrouching())
		{
			PlayIntimidationMontage();
		}
		else
		{

			PlayBeginChaseMontage();
		}
	}

	EnemyState = EEnemyState::EES_Chasing;

	if (bIsRunning)
	{
		SetMovementAttributes(false, true, false, ChasingSpeedRun);
	}
	else
	{
		SetMovementAttributes(false, true, false, ChasingSpeedWalk);
	}

	ClearAttackStrafeTimer();
	ClearSecondChaseStrafeTimer();
	ClearThirdChaseStrafeTimer();
	ClearChaseOrStrafeTimer();
	MoveToTarget(CombatTarget);
	StartChaseActionTimer();
	StartChaseSpeedTimer();
	StartChaseStrafeTimer();
	StartChaseDodgeTimer();

	//UE_LOG(LogTemp, Warning, TEXT("Chase!"));
}

void AEnemy::StartChaseActionTimer()
{
	if (!IsChasing()) return;

	const float ChaseActionDecisionTime = FMath::RandRange(ChaseActionDecisionMin, ChaseActionDecisionMax);
	GetWorldTimerManager().SetTimer(ChaseActionTimer, this, &AEnemy::PerformChaseAction, ChaseActionDecisionTime);
}

void AEnemy::PerformChaseAction()
{
	if (!IsChasing()) return;

	PlayChaseActionMontage();
	ClearChaseActionTimer();
	StartChaseActionTimer();
}

void AEnemy::StartChaseSpeedTimer()
{
	if (!IsChasing()) return;

	const float ChaseSpeedTime = FMath::RandRange(ChaseSpeedMin, ChaseSpeedMax);
	GetWorldTimerManager().SetTimer(ChaseSpeedTimer, this, &AEnemy::PerformChaseSpeedChange, ChaseSpeedTime);
}

void AEnemy::StartChaseStrafeTimer()
{
	if (!IsChasing()) return;

	const float ChaseStrafeTime = FMath::RandRange(ChaseStrafeMin, ChaseStrafeMax);
	GetWorldTimerManager().SetTimer(ChaseStrafeTimer, this, &AEnemy::PerformChaseStrafe, ChaseStrafeTime);
}

void AEnemy::StartSecondChaseStrafeTimer()
{
	if (!IsChasing()) return;

	const float ChaseStrafeTime = FMath::RandRange(NextChaseStrafeMin, NextChaseStrafeMax);
	GetWorldTimerManager().SetTimer(SecondChaseStrafeTimer, this, &AEnemy::PerformSecondChaseStrafe, ChaseStrafeTime);
}

void AEnemy::StartThirdChaseStrafeTimer()
{
	if (!IsChasing()) return;

	const float ChaseStrafeTime = FMath::RandRange(NextChaseStrafeMin, NextChaseStrafeMax);
	GetWorldTimerManager().SetTimer(ThirdChaseStrafeTimer, this, &AEnemy::PerformThirdChaseStrafe, ChaseStrafeTime);
}

void AEnemy::StartChaseDodgeTimer()
{
	if (!IsChasing()) return;

	const float DodgeTime = FMath::RandRange(DodgeMin, DodgeMax);
	GetWorldTimerManager().SetTimer(DodgeTimer, this, &AEnemy::PerformChaseDodge, DodgeTime);
}

void AEnemy::PerformChaseDodge()
{
	if (!IsChasing()) return;

	FName Section;

	PlayDodgeMontage(Section);

	ClearChaseDodgeTimer();
	StartChaseDodgeTimer();
}

void AEnemy::PerformEvade()
{
	if (CombatTarget)
	{
		FName Section;
		const int32 Chance = FMath::RandRange(0,EvadeChance);

		switch (Chance)
		{
		case 0:
			PlayDodgeMontage(Section);
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}
	}
}

void AEnemy::PerformChaseSpeedChange()
{
	if (!IsChasing()) return;

	/*const int32 Selection = FMath::RandRange(0, 1);
	
	Selection == 1 ? GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed : GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;*/

	if (bIsRunning)
	{
		bIsRunning = false;
		SetMovementAttributes(false, true, false, ChasingSpeedWalk);
	}
	else
	{
		bIsRunning = true;
		ChaseTarget();
	}
	
	ClearChaseSpeedTimer();
	StartChaseSpeedTimer();
}

void AEnemy::PerformChaseStrafe()
{
	if (bIsRunning) return;

	SetMovementAttributes(true, false, true, StrafingSpeed);

	MoveToLocation(GetStrafeLocation());
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetStrafeLocation(), FColor::Red, false, 5.f);

	ClearSecondChaseStrafeTimer();
	StartSecondChaseStrafeTimer();

	UE_LOG(LogTemp, Warning, TEXT("First Strafe"));
}

void AEnemy::PerformSecondChaseStrafe()
{
	if (bIsRunning) return;

	SetMovementAttributes(true, false, true, StrafingSpeed);

	MoveToLocation(GetStrafeLocation());
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetStrafeLocation(), FColor::Red, false, 5.f);

	ClearThirdChaseStrafeTimer();
	StartThirdChaseStrafeTimer();

	UE_LOG(LogTemp, Warning, TEXT("Second Strafe"));
}

void AEnemy::PerformThirdChaseStrafe()
{
	if (bIsRunning) return;

	SetMovementAttributes(true, false, true, StrafingSpeed);

	MoveToLocation(GetStrafeLocation());
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetStrafeLocation(), FColor::Red, false, 5.f);

	ClearThirdChaseStrafeTimer();
	StartChaseOrStrafeTimer();

	UE_LOG(LogTemp, Warning, TEXT("Third Strafe"));
}

void AEnemy::StartChaseOrStrafeTimer()
{
	if (!IsChasing()) return;

	const float ChaseOrStrafeTime = FMath::RandRange(ChaseOrStrafeMin, ChaseOrStrafeMax);

	GetWorldTimerManager().SetTimer(ChaseOrStrafeTimer, this, &AEnemy::ChaseOrStrafeDecision, ChaseOrStrafeTime);
}

void AEnemy::ChaseOrStrafeDecision()
{
	const int32 Decision = FMath::RandRange(0, 1);

	switch (Decision)
	{
	case 0:
		PerformSecondChaseStrafe();
		break;
	case 1:
		ChaseTarget();
		break;
	default:
		PerformThirdChaseStrafe();
		break;
	}
}

void AEnemy::PerformAttackStrafe()
{
	if (IsChasing()) return;

	SetMovementAttributes(true, false, true, StrafingSpeed);

	MoveToLocation(GetStrafeLocation());

	//UE_LOG(LogTemp, Error, TEXT("AttackStrafe"));

	ClearAttackStrafeTimer();
	StartNextAttackStrafeTimer();
}

void AEnemy::StartFirstAttackStrafeTimer()
{
	if (IsChasing()) return;

	GetWorldTimerManager().SetTimer(AttackStrafeTimer, this, &AEnemy::PerformAttackStrafe, FirstAttackStrafeTime);
}

void AEnemy::StartNextAttackStrafeTimer()
{
	if (IsChasing()) return;

	const float AttackStrafeTime = FMath::RandRange(AttackStrafeMin, AttackStrafeMax);

	GetWorldTimerManager().SetTimer(AttackStrafeTimer, this, &AEnemy::PerformAttackStrafe, AttackStrafeTime);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackStrafeRadius()
{
	return InTargetRange(CombatTarget, AttackStrafeRadius);
}

bool AEnemy::IsPatrolling()
{
	return EnemyState == EEnemyState::EES_Patrolling;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearChaseActionTimer()
{
	GetWorldTimerManager().ClearTimer(ChaseActionTimer);
}

void AEnemy::ClearChaseDodgeTimer()
{
	GetWorldTimerManager().ClearTimer(DodgeTimer);
}

void AEnemy::ClearChaseSpeedTimer()
{
	GetWorldTimerManager().ClearTimer(ChaseSpeedTimer);
}

void AEnemy::ClearChaseStrafeTimer()
{
	GetWorldTimerManager().ClearTimer(ChaseStrafeTimer);
}

void AEnemy::ClearSecondChaseStrafeTimer()
{
	GetWorldTimerManager().ClearTimer(SecondChaseStrafeTimer);
}

void AEnemy::ClearThirdChaseStrafeTimer()
{
	GetWorldTimerManager().ClearTimer(ThirdChaseStrafeTimer);
}

void AEnemy::ClearChaseOrStrafeTimer()
{
	GetWorldTimerManager().ClearTimer(ChaseOrStrafeTimer);
}

void AEnemy::ClearAttackStrafeTimer()
{
	GetWorldTimerManager().ClearTimer(AttackStrafeTimer);
}

void AEnemy::SetMovementAttributes(bool IsStrafing, bool OrientMovement, bool UseDesiredRot, float Speed)
{
	bIsStrafing = IsStrafing;
	GetCharacterMovement()->bOrientRotationToMovement = OrientMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = UseDesiredRot;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

FVector AEnemy::GetStrafeLocation()
{
	const int32 LeftOrRightStrafe = FMath::RandRange(0, 1);

	switch (LeftOrRightStrafe)
	{
	case 0:
		return GetActorLocation() + (GetActorForwardVector() * 200.f) + (GetActorRightVector() * 1000.f);
		break;
	case 1:
		return GetActorLocation() + (GetActorForwardVector() * 200.f) + (GetActorRightVector() * -1000.f);
		break;
	default:
		return GetActorLocation() + (GetActorForwardVector() * 200.f) + (GetActorRightVector() * 1000.f);
		break;
	}
}

void AEnemy::MoveToLocation(FVector Location)
{
	if (EnemyController == nullptr || Location == FVector(0,0,0)) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(StrafeAcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::FocusOnTarget()
{
	EnemyController->SetFocus(CombatTarget);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&
		!SeenPawn->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::StartChasingTarget()
{
	if (CombatTarget)
	{
		ChaseTarget();
	}
	else
	{
		StartPatrolling();
	}
}