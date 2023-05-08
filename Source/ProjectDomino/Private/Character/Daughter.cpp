// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Daughter.h"
#include "GameFramework/CharacterMovementComponent.h"

ADaughter::ADaughter() :
	CrouchTime(1.f)
{
}

void ADaughter::SpaceAction()
{
	if (IsOccupied()) return;

	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
		ActionState = EActionState::EAS_StartEndCrouch;
		StartCrouchTimer();
	}
	else if (!bIsRunning)
	{
		Crouch();
		ActionState = EActionState::EAS_StartEndCrouch;
		StartCrouchTimer();
	}
}

void ADaughter::Combo()
{
	if (ComboSection == FName("Attack1"))
	{
		ComboSection = FName("Attack2");
	}
	else if (ComboSection == FName("Attack2"))
	{
		ComboSection = FName("Attack1");
	}
}

void ADaughter::StartCrouchTimer()
{
	GetWorldTimerManager().SetTimer(CrouchTimer, this, &ADaughter::CanMoveAgain, CrouchTime);
}

void ADaughter::CanMoveAgain()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ADaughter::SpawnDefaultWeapon()
{

}