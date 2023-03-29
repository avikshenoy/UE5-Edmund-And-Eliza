// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDOMINO_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);

private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
};
