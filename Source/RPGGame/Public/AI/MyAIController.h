// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class RPGGAME_API AMyAIController : public AAIController
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
