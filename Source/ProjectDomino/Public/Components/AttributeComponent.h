// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTDOMINO_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* Current Health */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float Health;

	/* Max Health */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float MaxHealth;

	/* Current Stamina */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float Stamina;

	/* Max Stamina */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float DodgeCost;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float LightAttackCost;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float StrongAttackCost;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Actor Attributes")
		float StaminaRegenRate;

private:

public:	

	void RecieveDamage(float Damage);
	void UseStamina(float StaminaCost);
	void Heal(float HealAmount);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();

	/** Getters */
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetLightAttackCost() const { return LightAttackCost; }
	FORCEINLINE float GetStrongAttackCost() const { return StrongAttackCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }

};
