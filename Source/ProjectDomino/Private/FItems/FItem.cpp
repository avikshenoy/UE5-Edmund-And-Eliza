// Fill out your copyright notice in the Description page of Project Settings.


#include "FItems/FItem.h"

UFItem::UFItem()
{
	ItemName = FText::FromString("DefaultItemName");
	UseActionText = FText::FromString("DefaultActionText");
	ItemDescription = FText::FromString("DefualtItemDescription");
}

void UFItem::Use(APlayerCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Define Use function for FItem, might want to make this pure virtual"));
}
