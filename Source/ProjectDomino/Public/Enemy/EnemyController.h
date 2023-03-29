// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBlackboardComponent;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class PROJECTDOMINO_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeComponent;

public:
	
	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() const { return BlackboardComponent; }
};
