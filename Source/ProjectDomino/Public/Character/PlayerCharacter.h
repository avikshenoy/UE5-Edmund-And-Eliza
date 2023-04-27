// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class UPlayerOverlay;

UCLASS()
class PROJECTDOMINO_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	APlayerCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void PlayHitCameraShake();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** </AActor> */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* MyInventoryComponent;

	UFUNCTION(BlueprintCallable)
	void UseItem(class UFItem* FItem);

protected:
	
	/** <AActor> */
	virtual void BeginPlay() override;
	/** <AActor> */

	/** Callbacks for input */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnRightOrLeft(float Value);
	void StopTurningRightOrLeft();
	void QuickTurn();
	void PickupKeyPressed();
	virtual void Attack() override;
	void PlayAttackCameraShake();
	void ChanceToPlayGruntSound();
	virtual void StrongAttack() override;
	void ClearComboResetTimer();
	virtual void SpaceAction();
	//void DodgeR();
	//void DodgeL();
	void StartSearchMode();
	void StopSearchMode();
	void LookBehindPlayer();
	void Run();
	void StopRunning();
	void ArmOrDisarm();

	/** Combat */
	virtual bool CanAttack(const FName& TypeOfAttack) override;
	virtual void AttackEnd() override;
	void StartComboTimer();
	virtual void Combo();
	void ResetCombo();
	virtual void DodgeEnd() override;
	virtual void QuickTurnEnd() override;
	void Rotate180(float DeltaTime);
	void EquipWeapon(AWeapon* OverlappingWeapon);
	bool CanArm();
	bool CanDisarm();
	void Arm();
	void Disarm();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die() override;
	bool HasEnoughStamina(float StaminaCost);
	void AdjustCameraBoomOffset(float DeltaTime);
	void UseStamina(float StaminaCost);

	UFUNCTION(BlueprintImplementableEvent)
	void CameraFollowsPlayer();

	bool IsDodging();

	UFUNCTION(BlueprintCallable)
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	bool IsUnoccupied();

	UFUNCTION(BlueprintCallable)
	bool IsSearching();
	
	UFUNCTION(BlueprintCallable)
	bool IsInInventory();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	virtual void SpawnDefaultWeapon() override;

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void ReturnToMainMenuAfterDeath();

	/* Take Key pressed, call when take key is pressed, Same functionality with PickupKeyPressed so merge later down the line*/
	void TakeKeyPressed();

	void SearchMode(float DeltaTime);


	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MoveForwardValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MoveRightValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsRunning;

	FName ComboSection;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> HitCameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> AttackCameraShakeClass;

private:

	void InitializePlayerOverlay();

	/** Components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CurrentArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float SearchArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ArmLengthSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DefaultBoomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CurrentBoomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float InterpedBoomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BoomOffsetSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float TurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float TurnInPlaceSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WalkingSteeringSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool TurnRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool TurnLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float QuickTurnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsLookingBehind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ComboTime;

	FTimerHandle ComboResetTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float PlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPlayerCharacterAnimInstance* PlayerCharacterAnimInstance;

public:

	void SetHUDHealth();

	/* Getters */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetMoveForwardValue() const { return MoveForwardValue; }
	FORCEINLINE float GetMoveRightValue() const { return MoveRightValue; }
	FORCEINLINE bool GetTurnRight() const { return TurnRight; }
	FORCEINLINE bool GetTurnLeft() const { return TurnLeft; }
	FORCEINLINE float GetTurnSpeed() const { return TurnSpeed; }
	FORCEINLINE float GetPlayerSpeed() const { return PlayerSpeed; }
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }
	FORCEINLINE bool GetIsRunning() const { return bIsRunning; }
	FORCEINLINE float GetTurnInPlaceSpeed() const { return TurnInPlaceSpeed; }
	FORCEINLINE float GetWalkingSteeringSpeed() const { return WalkingSteeringSpeed; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return Attributes; }
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() const { return PlayerOverlay; }

	/* Setters */
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE void SetTurnSpeed(float CharacterTurnSpeed) { TurnSpeed = CharacterTurnSpeed; }
	FORCEINLINE void SetPlayerSpeed(float CharacterSpeed) { PlayerSpeed = CharacterSpeed; }
	FORCEINLINE void SetIsRunning(bool Running) { bIsRunning = Running; }

};
