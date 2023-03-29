// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

enum class EItemState : uint8
{
	EIS_Unequipped,
	EIS_Equipped
};

UCLASS()
class PROJECTDOMINO_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/* Item's item state */
	EItemState ItemState = EItemState::EIS_Unequipped;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Handles what happens when player overlaps sphere. When player is within sphere radius, UI elements will pop up, and the player will be able to pick up the item */
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Handles what happens when player stops overlapping sphere. When player is not within sphere radius, UI elements will disapear, and the player will not be able to pick up the item */
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:


	/* Mesh for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;

	/* Sphere Collision for the item. When player is within sphere radius, UI elements will pop up, and the player will be able to pick up the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* PickupSphere;

public:	
	
	/* Getters */
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USphereComponent* GetPickupSphere() const { return PickupSphere; }
};
