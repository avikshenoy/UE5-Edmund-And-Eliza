// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Pickupable.h"
#include "Inventory/InventoryComponent.h"



APickupable::APickupable() {
	ItemID = FName("No ID");
}

//implementation for OnPickUp
/*NOTE: OnPickUp is a blueprint native function, so instead of defining it as OnPickUp here you have to-
define the default function as OnPickUp_Implementation(). In blueprint you can redefine it  to overwrite this*/
void APickupable::OnPickUp_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("I tried to pick this up haha :)"));
	Destroy();
}
