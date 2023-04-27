// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterAnimInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Item.h"
#include "Items/Pickupable.h"
#include "Inventory/InventoryComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"
#include "Components/AttributeComponent.h"

APlayerCharacter::APlayerCharacter() :
	TurnSpeed(50.f),
	TurnInPlaceSpeed(75.f),
	WalkingSteeringSpeed(50.f),
	QuickTurnSpeed(1.f),
	DefaultArmLength(200.f),
	CurrentArmLength(200.f),
	SearchArmLength(350.f),
	ArmLengthSpeed(10.f),
	DefaultBoomOffset(0.f),
	CurrentBoomOffset(0.f),
	InterpedBoomOffset(100.f),
	BoomOffsetSpeed(2.5),
	ComboSection(FName("Attack1")),
	ComboTime(0.5f),
	RunSpeed(600.f),
	WalkSpeed(200.f)
{
	PrimaryActorTick.bCanEverTick = true;

	/* Create subobject for Camera Boom */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultArmLength;
	CameraBoom->bUsePawnControlRotation = true;

	/* Create subobject for Follow Camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; 

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	/*Initialize player's inventory component*/
	MyInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void APlayerCharacter::UseItem(UFItem* FItem)
{
	if (FItem) {
		FItem->Use(this);
		FItem->OnUse(this); //bp event 
		MyInventoryComponent->RemoveItem(FItem);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && PlayerOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	CameraFollowsPlayer();
	StopTurningRightOrLeft();
	SearchMode(DeltaTime);
	Rotate180(DeltaTime);
	//AdjustCameraBoomOffset(DeltaTime);

	if (bIsRunning)
	{
		if (MoveForwardValue != 1.0)
		{
			StopRunning();
		}
	/*	if (CharacterState == ECharacterState::ECS_EquippedOnHandedWeapon)
		{
			StopRunning();
		}*/
		if (GetCharacterMovement()->IsCrouching())
		{
			StopRunning();
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT(" is %s"), b ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("Crouching is %s"), GetCharacterMovement()->IsCrouching() ? TEXT("true") : TEXT("false"));

	//UE_LOG(LogTemp, Warning, TEXT("MoveForward is %f"), MoveForwardValue);
	//UE_LOG(LogTemp, Warning, TEXT("MoveRight is %f"), MoveRightValue);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(ActionState));
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsDodging()) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
		ComboSection = FName("Attack1");
	}
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	PlayHitCameraShake();
}

void APlayerCharacter::PlayHitCameraShake()
{
	if (HitCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDodging()) return 0.f;

	HandleDamage(DamageAmount);

	SetHUDHealth();
	return DamageAmount;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	/** Axis mappings */
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	/** Action mappings */
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::PickupKeyPressed);
	/* Input for player picking up items  Remember you might want to merge this with Pick up down the line*/
	PlayerInputComponent->BindAction("Take", IE_Pressed, this, &APlayerCharacter::TakeKeyPressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Strong Attack", IE_Pressed, this, &APlayerCharacter::StrongAttack);
	PlayerInputComponent->BindAction("Space Action", IE_Pressed, this, &APlayerCharacter::SpaceAction);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::Run);
	//PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::StopRunning);
	//PlayerInputComponent->BindAction("Dodge R", IE_Pressed, this, &APlayerCharacter::DodgeR);
	//PlayerInputComponent->BindAction("Dodge L", IE_Pressed, this, &APlayerCharacter::DodgeL);
	PlayerInputComponent->BindAction("QuickTurn", IE_Pressed, this, &APlayerCharacter::QuickTurn);
	PlayerInputComponent->BindAction("Search", IE_Pressed, this, &APlayerCharacter::StartSearchMode);
	PlayerInputComponent->BindAction("Search", IE_Released, this, &APlayerCharacter::StopSearchMode);
	PlayerInputComponent->BindAction("LookBehind", IE_Pressed, this, &APlayerCharacter::LookBehindPlayer);
	PlayerInputComponent->BindAction("Arm/Disarm", IE_Pressed, this, &APlayerCharacter::ArmOrDisarm);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacterAnimInstance = Cast<UPlayerCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	
	Tags.Add(FName("EngageableTarget"));

	InitializePlayerOverlay();
	SpawnDefaultWeapon();
}

void APlayerCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		/** Original walk forward code */

		// Determine what direction player controller is facing
		/*const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);*/ // Value is 1 if player is moving forward and -1 if backward

		AddMovementInput(GetActorForwardVector(), Value);
	}

	MoveForwardValue = Value;
}

void APlayerCharacter::MoveRight(float Value)
{
	//if (ActionState != EActionState::EAS_Unoccupied) return;
	if (IsSearching() || IsUnoccupied())
	{
		if ((Controller != nullptr) && (Value != 0.0f))
		{
			/** Original Move Right Code */

			// Determine what direction player controller is facing
		/*	const FRotator Rotation{ Controller->GetControlRotation() };
			const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

			const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

			AddMovementInput(Direction, Value);*/ // Value is 1 if player is moving forward and -1 if backward

			FRotator DeltaRotation = FRotator::ZeroRotator;
			DeltaRotation.Yaw = Value * TurnSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);
			AddActorLocalRotation(DeltaRotation, true);

			TurnRightOrLeft(Value);
		}
	}


	MoveRightValue = Value;
}

void APlayerCharacter::Turn(float Value)
{
	if (ActionState != EActionState::EAS_Searching) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	if (ActionState != EActionState::EAS_Searching) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	AddControllerPitchInput(Value);
}

void APlayerCharacter::TurnRightOrLeft(float Value)
{
	if (!(PlayerCharacterAnimInstance->GetIsAccelerating()))
	{
		Value == 1.f ? TurnRight = true : TurnRight = false;
		Value == -1.f ? TurnLeft = true : TurnLeft = false;
	}
	else
	{
		TurnRight = false;
		TurnLeft = false;
	}
}

void APlayerCharacter::PickupKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon) // If player picks up weapon...
	{
		EquipWeapon(OverlappingWeapon);
	}
}

void APlayerCharacter::StopTurningRightOrLeft()
{
	if (MoveRightValue == 0.f)
	{
		TurnRight = false;
		TurnLeft = false;
	}
}

void APlayerCharacter::QuickTurn()
{
	if (IsOccupied()) return;

	if (GetMovementComponent()->IsCrouching())
	{
		PlayQuickTurnMontage(CrouchQuickTurnMontage);
	}
	else
	{
		PlayQuickTurnMontage(QuickTurnMontage);
	}
	ActionState = EActionState::EAS_QuickTurning;
}

void APlayerCharacter::TakeKeyPressed() {
	APickupable* OverLappingPickupable = Cast<APickupable>(OverlappingItem);

	if (OverLappingPickupable) // If player picks up a pickupable
	{

		//Might want to turn OverlappingItem into an array
		//Turn below line of code into an if statement, write additional return false in AddItem for when capacity is reached
		//such that only when additem is succesful will OverLappingPickupable->OnPickUp(); and OverlappingItem = nullptr; is run

		//get the FItem from Pickupable and add it to player's inventory
		if (MyInventoryComponent->AddItem(OverLappingPickupable->GetFItem())) {
			//trigger Onpickup function
			OverLappingPickupable->OnPickUp();
			OverlappingItem = nullptr;
		}
	}
}

void APlayerCharacter::Attack()
{
	Super::Attack();

	if (CanAttack(FName("Light")))
	{
		ClearComboResetTimer();
		StopMontage(AttackMontage);
		PlayPlayerAttackMontage(AttackMontage,ComboSection);
		EquippedWeapon->SetWeaponDamage(LightDamage);
		ActionState = EActionState::EAS_Attacking;
		UseStamina(Attributes->GetLightAttackCost());
		ChanceToPlayGruntSound();
		PlayAttackCameraShake();

		//UE_LOG(LogTemp, Warning, TEXT("Damage is %f"), EquippedWeapon->GetDamage());
	}
}

void APlayerCharacter::PlayAttackCameraShake()
{
	if (AttackCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(AttackCameraShakeClass);
	}
}

void APlayerCharacter::ChanceToPlayGruntSound()
{
	int GruntChance = FMath::RandRange(0, 1);

	switch (GruntChance)
	{
	case 0:
		PlayGruntSound(GetActorLocation());
		break;
	case 1:
		break;
	default:
		break;
	}
}

void APlayerCharacter::StrongAttack()
{
	Super::StrongAttack();

	if (CanAttack(FName("Strong")))
	{
		ClearComboResetTimer();
		StopMontage(AttackMontage);
		PlayGruntSound(GetActorLocation());
		PlayPlayerAttackMontage(StrongAttackMontage, ComboSection);
		EquippedWeapon->SetWeaponDamage(StrongDamage);
		ActionState = EActionState::EAS_Attacking;
		UseStamina(Attributes->GetStrongAttackCost());
		PlayAttackCameraShake();

		//UE_LOG(LogTemp, Error, TEXT("Damage is %f"), EquippedWeapon->GetDamage());

	}
}

void APlayerCharacter::ClearComboResetTimer()
{
	GetWorldTimerManager().ClearTimer(ComboResetTimer);
}

void APlayerCharacter::SpaceAction()
{

}

//void APlayerCharacter::DodgeR()
//{
//	if (IsOccupied() || !HasEnoughStamina()) return;
//
//	FName Section("Right");
//
//	PlayDodgeMontage(Section);
//	ActionState = EActionState::EAS_Dodge;
//	if (Attributes && PlayerOverlay)
//	{
//		Attributes->UseStamina(Attributes->GetDodgeCost());
//		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
//	}
//}
//
//void APlayerCharacter::DodgeL()
//{
//	if (IsOccupied() || !HasEnoughStamina()) return;
//
//	FName Section("Left");
//
//	PlayDodgeMontage(Section);
//	ActionState = EActionState::EAS_Dodge;
//	if (Attributes && PlayerOverlay)
//	{
//		Attributes->UseStamina(Attributes->GetDodgeCost());
//		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
//	}
//}

void APlayerCharacter::StartSearchMode()
{
	if (IsOccupied()) return;

	ActionState = EActionState::EAS_Searching;
}

void APlayerCharacter::StopSearchMode()
{
	if (IsSearching())
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void APlayerCharacter::LookBehindPlayer()
{
	if (IsOccupied()) return;

	if (bIsLookingBehind)
	{
		bIsLookingBehind = false;
	}
	else
	{
		bIsLookingBehind = true;
	}
}

void APlayerCharacter::Run()
{
	if (bIsRunning)
	{
		StopRunning();
	}
	else if (MoveForwardValue == 1.f)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	//&& CharacterState == ECharacterState::ECS_Unequipped
}

void APlayerCharacter::StopRunning()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::ArmOrDisarm()
{
	if (CanDisarm())
	{
		Disarm();
	}
	else if (CanArm())
	{
		Arm();
	}
}

bool APlayerCharacter::CanAttack(const FName& TypeOfAttack)
{
	if (TypeOfAttack == FName("Light"))
	{
		return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped && 
			HasEnoughStamina(Attributes->GetLightAttackCost()) &&
			ComboSection.GetStringLength() > 0 &&
			!GetCharacterMovement()->IsCrouching();
	}
	else if (TypeOfAttack == FName("Strong"))
	{
		return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped &&
			HasEnoughStamina(Attributes->GetStrongAttackCost()) &&
			ComboSection.GetStringLength() > 0 &&
			!GetCharacterMovement()->IsCrouching();
	}

	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped &&
		HasEnoughStamina(Attributes->GetLightAttackCost()) &&
		ComboSection.GetStringLength() > 0 &&
		!GetCharacterMovement()->IsCrouching();
}

void APlayerCharacter::AttackEnd()
{
	Combo();
	StartComboTimer();
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::StartComboTimer()
{
	GetWorldTimerManager().SetTimer(ComboResetTimer, this, &APlayerCharacter::ResetCombo, ComboTime);
}

void APlayerCharacter::Combo()
{
	if (ComboSection == FName("Attack1"))
	{
		ComboSection = FName("Attack2");
	}
	else if (ComboSection == FName("Attack2"))
	{
		ComboSection = FName("Attack3");
	}
	else if (ComboSection == FName("Attack3"))
	{
		ComboSection = FName("Attack1");
	}
}

void APlayerCharacter::ResetCombo()
{
	ComboSection = FName("Attack1");
	EquippedWeapon->SetWeaponDamage(LightDamage);
}

void APlayerCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::QuickTurnEnd()
{
	Super::QuickTurnEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::Rotate180(float DeltaTime)
{
	if (ActionState == EActionState::EAS_QuickTurning)
	{
		FRotator NewRot = GetActorRotation();
		float InterpRot = FMath::FInterpTo(0.f, 155.f, DeltaTime, QuickTurnSpeed);
		NewRot.Yaw += InterpRot;
		SetActorRotation(NewRot);
	}

	/*FRotator NewRot = GetActorRotation();
	NewRot.Yaw += 180;

	FRotator InterpRot = FMath::RInterpTo(GetActorRotation(), NewRot, DeltaTime, QuickTurnSpeed);
	SetActorRotation(InterpRot);*/

}

void APlayerCharacter::EquipWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this); // Attaches the weapon to right hand
	CharacterState = ECharacterState::ECS_EquippedOnHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = OverlappingWeapon;
}

bool APlayerCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

bool APlayerCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

void APlayerCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOnHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void APlayerCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();

	ReturnToMainMenuAfterDeath();
}

bool APlayerCharacter::HasEnoughStamina(float StaminaCost)
{
	return Attributes && Attributes->GetStamina() > StaminaCost;
}

void APlayerCharacter::AdjustCameraBoomOffset(float DeltaTime)
{
	if (CharacterState == ECharacterState::ECS_EquippedOnHandedWeapon)
	{
		CurrentBoomOffset = FMath::FInterpTo(CurrentBoomOffset, InterpedBoomOffset, DeltaTime, BoomOffsetSpeed);
	}
	else
	{
		CurrentBoomOffset = FMath::FInterpTo(CurrentBoomOffset, DefaultBoomOffset, DeltaTime, BoomOffsetSpeed);
	}

	CameraBoom->SocketOffset.Y = CurrentBoomOffset;
}

void APlayerCharacter::UseStamina(float StaminaCost)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->UseStamina(StaminaCost);
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool APlayerCharacter::IsDodging()
{
	return ActionState == EActionState::EAS_Dodge;
}

bool APlayerCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::IsSearching()
{
	return ActionState == EActionState::EAS_Searching;
}

bool APlayerCharacter::IsInInventory()
{
	return ActionState == EActionState::EAS_InInventory;
}

void APlayerCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APlayerCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();

	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("SpineSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void APlayerCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::SearchMode(float DeltaTime)
{
	if (IsSearching())
	{
		CurrentArmLength = FMath::FInterpTo(CurrentArmLength, SearchArmLength, DeltaTime, ArmLengthSpeed);
	}
	else
	{
		CurrentArmLength = FMath::FInterpTo(CurrentArmLength, DefaultArmLength, DeltaTime, ArmLengthSpeed);
	}

	CameraBoom->TargetArmLength = CurrentArmLength;
}

void APlayerCharacter::InitializePlayerOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(.5f);
			}
		}
	}
}

void APlayerCharacter::SetHUDHealth()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

