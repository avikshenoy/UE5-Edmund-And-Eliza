// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Character/PlayerCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Item : DefaultItems) {
		AddItem(Item);
	}

}

bool UInventoryComponent::AddItem(UFItem* Item)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), capacity);
	if (!Item) {
		UE_LOG(LogTemp, Warning, TEXT("AddItem Fitem doesnt exist"));
		return false;
	}
	if (Items.Num() + 1 > capacity) {
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
		return false;
	}
	Item->CurrentInventory = this;
	Item->World = GetWorld();
	Items.Add(Item);

	//Update UI here
	OnInventoryUpdated.Broadcast();
	DebugItemList();

	return true;
}

bool UInventoryComponent::RemoveItem(UFItem* Item)
{
	if (!Item) {
		UE_LOG(LogTemp, Warning, TEXT("RemoveItem Fitem doesnt exist"));
		return false;
	}
	Item->CurrentInventory = nullptr;
	Item->World = nullptr;
	Items.RemoveSingle(Item);

	//Update UI here
	OnInventoryUpdated.Broadcast();
	DebugItemList();

	return true;
}


void UInventoryComponent::DebugItemList()
{
	UE_LOG(LogTemp, Warning, TEXT("Current Inventory List"));
	for (auto& Item : Items) {
		FString ItemNameString = Item->ItemName.ToString();
		FString s = Item->ItemName.ToString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *s);
	}
}
