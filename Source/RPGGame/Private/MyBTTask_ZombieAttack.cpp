// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ZombieAttack.h"

#include "AIController.h"
#include "MyAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UMyBTTask_ZombieAttack::UMyBTTask_ZombieAttack()
{
	
}

EBTNodeResult::Type UMyBTTask_ZombieAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//获取AI控制器，通过传入的行为树组件
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		//通过黑板组件获得目标Actor
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if (!UMyAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}
		//播放攻击动画
		
		UAnimInstance* Instance = MyPawn->GetMesh()->GetAnimInstance();
		//如果收招则从第一招开始打，并设置为连击状态
		
		Instance->Montage_Play(AttackAnim);
		//随机选择一个攻击方式
		switch (FMath::RandRange(0,2)) {
		case 0 :
			Instance->Montage_JumpToSection("MeleeAttack_A");
			break;
		case 1 : 
			Instance->Montage_JumpToSection("MeleeAttack_B");
			break;
		default : 
			Instance->Montage_JumpToSection("MeleeAttack_C");
			break;
		}
		//射线检测是否打到
		
		/*//通过AIController获得AIPawn身上的骨骼位置，以及投射物发射方向
		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("gun_foregrip");
		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		// Ignore negative pitch to not hit the floor in front itself
		MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;

		//播放开枪动画
		MyPawn->PlayAnimMontage(AttackAnim);
		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);*/

		//如果 返回Succeeded
		return Instance ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}


