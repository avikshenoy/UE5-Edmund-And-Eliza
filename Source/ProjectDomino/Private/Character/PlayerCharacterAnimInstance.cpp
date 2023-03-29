// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacterAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

UPlayerCharacterAnimInstance::UPlayerCharacterAnimInstance()
{
}

// Called every frame 
void UPlayerCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PlayerCharacter)
	{
		GetStates();

		GetGroundSpeed();

		IsInAir();

		IsAccelerating();

		IsCrouching();

		GetMovementOffsetYaw();

		//SetActorRotToControlRot(GroundSpeed, DeltaTime, bIsAccelerating);

		GetMovementInputs();

		GetTurnBools();

		ChangeTurnSpeed(bIsAccelerating);

		//UE_LOG(LogTemp, Warning, TEXT("Crouched is %s"), (bIsCrouching ? TEXT("true") : TEXT("false")));
	}
}

void UPlayerCharacterAnimInstance::GetStates()
{
	CharacterState = PlayerCharacter->GetCharacterState();

	ActionState = PlayerCharacter->GetActionState();

	DeathPose = PlayerCharacter->GetDeathPose();
}

void UPlayerCharacterAnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerMovementComponent->Velocity);
}

void UPlayerCharacterAnimInstance::IsInAir()
{
	bIsInAir = PlayerMovementComponent->IsFalling();
}

void UPlayerCharacterAnimInstance::IsAccelerating()
{
	if (PlayerMovementComponent->GetCurrentAcceleration().Size() > 0.f)
	{
		bIsAccelerating = true;
	}
	else
	{
		bIsAccelerating = false;
	}
}

void UPlayerCharacterAnimInstance::IsCrouching()
{
	bIsCrouching = TryGetPawnOwner()->GetMovementComponent()->IsCrouching();
}

void UPlayerCharacterAnimInstance::GetMovementOffsetYaw()
{
	FRotator AimRotation = PlayerCharacter->GetActorRotation(); // Used to be GetActorBaseAimRotation()
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());

	// Movement offset yaw gives us the difference between the player's movement rotation and aim rotation. With this information, we know what walking animation to play
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	if (PlayerCharacter->GetVelocity().Size() > 0.f)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}
}

void UPlayerCharacterAnimInstance::GetMovementInputs()
{
	InputForward = PlayerCharacter->GetMoveForwardValue();
	InputRight = PlayerCharacter->GetMoveRightValue();
}

void UPlayerCharacterAnimInstance::GetTurnBools()
{
	TurningRight = PlayerCharacter->GetTurnRight();
	TurningLeft = PlayerCharacter->GetTurnLeft();
}

// Called at the start of the game
void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{
		PlayerMovementComponent = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerCharacterAnimInstance::ChangeTurnSpeed(bool IsAccelerating)
{
	IsAccelerating ? PlayerCharacter->SetTurnSpeed(PlayerCharacter->GetWalkingSteeringSpeed()) : PlayerCharacter->SetTurnSpeed(PlayerCharacter->GetTurnInPlaceSpeed());
}

void UPlayerCharacterAnimInstance::UseControllerYawOrNah(bool IsAccelerating, float PlayerSpeed)
{
	if (IsAccelerating && PlayerSpeed > 299.f) // When player is finished turning around, bUseControllerYaw is set to true
	{
		PlayerCharacter->bUseControllerRotationYaw = true;
	}
	else if (!IsAccelerating) // When player stops moving, bUseControllerYaw is set to false
	{
		PlayerCharacter->bUseControllerRotationYaw = false;
	}
}

/**
*	TODO: Instead of using SetActorRotToControlRot, experiment with creating a new walk start animation that plays if the player is facing the opposite direction they want to walk in.
*	There are some bugs with the current implementation of SetActorRotToControlRot, so it would be a good idea to try a better solution later in development.
*/

/** 
*	Sets Player's rotation to the controller's rotation at an interped speed based on circumstances.
*	Used for smoothly turning the player around if the player wants to go in the opposite direction they are currently facing
*/
void UPlayerCharacterAnimInstance::SetActorRotToControlRot(float PlayerSpeed, float DeltaTime, bool IsAccelerating)
{
	// Interps current actor rotation to control rotation based on CharacterTurnSpeed
	FRotator InterpedRotation = FMath::RInterpTo(
		PlayerCharacter->GetActorRotation(),
		PlayerCharacter->GetControlRotation(),
		DeltaTime,
		PlayerCharacter->GetTurnSpeed()
	);

	UseControllerYawOrNah(IsAccelerating, PlayerSpeed);

	// If player starts moving, interpolate current actor rotation to control rotation. bUseControllerRotationYaw must be false
	if (PlayerSpeed > 10.f && PlayerSpeed < 299.f && !(PlayerCharacter->bUseControllerRotationYaw))
	{
		PlayerCharacter->SetActorRotation(FRotator(0, InterpedRotation.Yaw, 0));
	}
}

	/*FString SpeedMessage = FString::Printf(TEXT("Speed is: %f"), GroundSpeed);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, SpeedMessage);
	}*/