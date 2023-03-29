// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{

}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Enemy = Cast<AEnemy>(TryGetPawnOwner());

	if (Enemy)
	{
		EnemyMovementComponent = Enemy->GetCharacterMovement();
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy)
	{
		GetMovementOffsetYaw();

		//UE_LOG(LogTemp, Warning, TEXT("Crouched is %s"), (bIsCrouching ? TEXT("true") : TEXT("false")));
	}
}

void UEnemyAnimInstance::GetMovementOffsetYaw()
{
	FRotator AimRotation = Enemy->GetActorRotation(); // Used to be GetBaseAimRotation()
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Enemy->GetVelocity());

	// Movement offset yaw gives us the difference between the player's movement rotation and aim rotation. With this information, we know what walking animation to play
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	if (Enemy->GetVelocity().Size() > 0.f)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}
}
