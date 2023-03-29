// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FItems/FItem.h"
#include "FHealthItem.generated.h"

/**
 *
 */
UCLASS()
class PROJECTDOMINO_API UFHealthItem : public UFItem
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FoodItem")
		float ItemHealAmount;

protected:

	void FHealthItem();

	virtual void Use(class APlayerCharacter* Character) override;

};
