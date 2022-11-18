// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyBTService_CheckAttackRange.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"




UMyBTService_CheckAttackRange::UMyBTService_CheckAttackRange()
{
	MaxAttackRange = 2000.f;
}


void UMyBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between ai pawn and target actor
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();

			APawn* AIPawn = MyController->GetPawn();
			if (ensure(AIPawn))
			{
				float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

				bool bWithinRange = DistanceTo < MaxAttackRange;

				bool bHasLOS = false;
				if (bWithinRange)
				{
					bHasLOS = MyController->LineOfSightTo(TargetActor);
				}

				//设置黑板中一个bool变量的值
				BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
			}
		}
	}
}
