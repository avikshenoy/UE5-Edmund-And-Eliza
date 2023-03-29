// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FItems/FItem.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

//delegate? Blueprints will bind to this to update the UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTDOMINO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	virtual void BeginPlay() override;

	//UPROPERTY(Blueprintcallable)
	bool AddItem(class UFItem* Item);
	//UPROPERTY(Blueprintcallable)
	bool RemoveItem(class UFItem* Item);

	//default items that you start the game with
	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<class UFItem*> DefaultItems;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryUpdated OnInventoryUpdated;

	//the items that you have in your inventory currently
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		TArray<class UFItem*> Items;

	//Item capacity (how many items can you store)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		int capacity = 8;


protected:

	void DebugItemList();

};
