// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPowerupActor.h"
#include "MyPowerupActor_Credits.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API AMyPowerupActor_Credits : public AMyPowerupActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 CreditsAmount;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;

	AMyPowerupActor_Credits();
};
