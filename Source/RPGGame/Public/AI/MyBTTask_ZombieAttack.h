// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTask_ZombieAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UMyBTTask_ZombieAttack : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere,Category="AI")
	UAnimMontage* AttackAnim;
	

public:

	UMyBTTask_ZombieAttack();
};
