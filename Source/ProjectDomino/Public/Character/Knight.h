// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PlayerCharacter.h"
#include "Knight.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDOMINO_API AKnight : public APlayerCharacter
{
	GENERATED_BODY()

public:
	
	AKnight();

protected:

	virtual void SpaceAction() override;
	virtual void SpawnDefaultWeapon() override;

private:

};
