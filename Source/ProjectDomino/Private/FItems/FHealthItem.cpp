// Fill out your copyright notice in the Description page of Project Settings.


#include "FItems/FHealthItem.h"
#include "Character/BaseCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Components/AttributeComponent.h"

void UFHealthItem::Use(class APlayerCharacter* Character) {
	if (Character) {
		UE_LOG(LogTemp, Warning, TEXT("Using heal item"));
		Character->GetAttributeComponent()->Heal(ItemHealAmount);
		Character->SetHUDHealth();
	}
}


