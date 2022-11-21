// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPowerupActor.h"
#include "MyPowerupActor_HealthPotion.generated.h"


/**
 * 
 */
UCLASS()
class RPGGAME_API AMyPowerupActor_HealthPotion : public AMyPowerupActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "HealthPotion")
	int32 CreditCost;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

	AMyPowerupActor_HealthPotion();
};
