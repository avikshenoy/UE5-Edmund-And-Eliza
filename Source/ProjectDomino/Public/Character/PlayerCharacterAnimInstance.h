// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "PlayerCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDOMINO_API UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPlayerCharacterAnimInstance();

	/* Updates animation data everyframe (like tick()) */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	void GetStates();

	void GetGroundSpeed();

	void IsInAir();

	void IsAccelerating();

	void IsCrouching();

	void GetMovementOffsetYaw();

	void GetMovementInputs();

	void GetTurnBools();

	/* Updates animation data before first frame (like beginplay()) */
	virtual void NativeInitializeAnimation() override;

	void ChangeTurnSpeed(bool IsAccelerating);

	/* Instantly Sets Player's rotation to the controller's rotation based on circumstances */
	void UseControllerYawOrNah(bool IsAccelerating, float PlayerSpeed);

	/* Sets Player's rotation to the controller's rotation at an interped speed based on circumstances.
	Used for smoothly turning the player around if the player wants to go in the opposite direction they are currently facing */
	void SetActorRotToControlRot(float PlayerSpeed, float DeltaTime, bool IsAccelerating);

private:


	/* Speed of the player character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	/* True if player is in air, false if grounded */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/* True if player is moving, false if standing still */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	/* Offset Yaw used for strafing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/* Offset yaw the frame before we stopped moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float InputForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float InputRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool TurningRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool TurningLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* PlayerMovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDeathPose> DeathPose;


public:

	/** Getters */
	FORCEINLINE bool GetIsAccelerating() const { return bIsAccelerating; }

};
