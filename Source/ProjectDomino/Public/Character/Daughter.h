// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PlayerCharacter.h"
#include "Daughter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDOMINO_API ADaughter : public APlayerCharacter
{
	GENERATED_BODY()

public:

	ADaughter();

protected:

	virtual void SpaceAction() override;
	virtual void Combo() override;

	void StartCrouchTimer();
	void CanMoveAgain();

private:

	FTimerHandle CrouchTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CrouchTime;
	
};
