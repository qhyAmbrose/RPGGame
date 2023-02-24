// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimNotifyState_MeleeAttack.h"
// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "MyCharacter.h"

void UMyAnimNotifyState_MeleeAttack::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Player = Cast<AMyCharacter>(MeshComp->GetOwner());
	if(Player)
	{ 
	ActorsToIgnore = { MeshComp->GetOwner() };
	LastLocation1 = Weapon->GetSocketLocation("Trace1");
	LastLocation2 = Weapon->GetSocketLocation("Trace2");
	}
}


void UMyAnimNotifyState_MeleeAttack::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if(Player)//检测是否报空//
	{
	//射线检测//
	UKismetSystemLibrary::BoxTraceMulti(Player->GetWorld(), LastLocation1, Weapon->GetSocketLocation("Trace1"), FVector(5, 30, 50), Weapon->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
	for (int i = 0; i < HitResult.Num(); i++)
	{
		AActor*HitActor = HitResult[i].GetActor();//获取本次射线击中的Actor//
		if (!HitActors.Contains(HitActor))//查询数组中是否有本次击中的Actor，如果没有则添加进数组并调用自带伤害函数，防止一次通知内多次击中的情况//
		{
			HitActors.Add(HitActor);
			UGameplayStatics::ApplyDamage(HitActor, 10.f, EventInstigator, Player,DamageTypeClass);
		}
	}
	//第二个插槽//
	UKismetSystemLibrary::BoxTraceMulti(Player->GetWorld(), LastLocation2, Weapon->GetSocketLocation("Trace2"), FVector(5, 30, 50), Weapon->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
	for (int i = 0; i < HitResult.Num(); i++)
	{
		AActor*HitActor = HitResult[i].GetActor();
		if (!HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			UGameplayStatics::ApplyDamage(HitActor, 10.f, EventInstigator, Player, DamageTypeClass);
		}
	}
	//一次Tick过后更新当前插槽的位置变量，下一次再与存储的变量做差值//
	LastLocation1 = Weapon->GetSocketLocation("Trace1");
	LastLocation2 = Weapon->GetSocketLocation("Trace2");
	}
}

void UMyAnimNotifyState_MeleeAttack::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	HitActors.Empty();
}

