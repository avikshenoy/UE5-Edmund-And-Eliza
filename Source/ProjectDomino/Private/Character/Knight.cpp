// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Knight.h"
#include "Components/AttributeComponent.h"


AKnight::AKnight() 
{

}

void AKnight::SpaceAction()
{
	if (IsOccupied() || !HasEnoughStamina(Attributes->GetDodgeCost())) return;

	FName Section("Backward");


	if (MoveRightValue == 1.0)
	{
		Section = FName("Right");
	}
	else if (MoveRightValue == -1.0)
	{
		Section = FName("Left");
	}
	else if (MoveForwardValue == 1.0)
	{
		Section = FName("Forward");
	}
	else if (MoveForwardValue == -1.0)
	{
		Section = FName("Backward");
	}

	PlayDodgeMontage(Section);
	ActionState = EActionState::EAS_Dodge;
	UseStamina(Attributes->GetDodgeCost());
	ChanceToPlayGruntSound();
}
