// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FItem.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInLineNew, DefaultToInstanced)
class PROJECTDOMINO_API UFItem : public UObject
{
	GENERATED_BODY()

public:
	UFItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
		class UWorld* World;


	//text for action prompt
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText UseActionText;

	//mesh for item on ground (you dont need this I think)
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	//	class UStaticMesh* Mesh;

	//thumbnail, might want to use 3d item thumbnail or something to generate
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		class UTexture2D* thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText ItemDescription;


	UPROPERTY()
		class UInventoryComponent* CurrentInventory;

	virtual void Use(class APlayerCharacter* Character);


	UFUNCTION(BlueprintImplementableEvent)
		void OnUse(class APlayerCharacter* Character);

};
