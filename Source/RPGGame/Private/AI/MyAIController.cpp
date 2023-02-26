// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (MyPawn)
	{
		GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
	
		GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	}
}

void AMyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//让AI朝向玩家
	/*ACharacter* MyPawn = Cast<ACharacter>(this->GetPawn());
	AActor* TargetActor = Cast<AActor>(this->GetBlackboardComponent()->GetValueAsObject("TargetActor"));

	FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("head");
	FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
	FRotator MuzzleRotation = Direction.Rotation();
	
	MyPawn->SetActorRotation(MuzzleRotation,ETeleportType::None);*/
}
