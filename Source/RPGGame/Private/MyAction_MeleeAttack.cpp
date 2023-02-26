// Fill out your copyright notice in the Description page of Project Settings.
#include "MyAction_MeleeAttack.h"
#include "MyCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UMyAction_MeleeAttack::UMyAction_MeleeAttack()
{
	
	/*StringTArray.Add("MeleeAttack_A");
	StringTArray.Add("MeleeAttack_B");
	StringTArray.Add("MeleeAttack_C");
	StringTArray.Add("MeleeAttack_D");*/
}

void UMyAction_MeleeAttack::StartAction_Implementation(AActor* Instigator)
{
	
	Super::StartAction_Implementation(Instigator);
	/*AMyCharacter* Character = Cast<AMyCharacter>(Instigator);
	UAnimInstance* Instance = Character->GetMesh()->GetAnimInstance();
	//如果收招则从第一招开始打，并设置为连击状态
	if(!Character->GetIsAttacking())
	{
		Instance->Montage_Play(AttackAnim);
		Character->SetIsAttacking(true);
	}
	//得到当前蒙太奇片段的名称，与构造函数中声明的数组一一对应
	SectionName=Instance->Montage_GetCurrentSection(AttackAnim);
	FString TargetName(SectionName.ToString());
	StringTArray.Find(TargetName, Index);
	//GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,TargetName);

	//当攻击A的时候按攻击键跳向B，。。。
	switch (Index) {
	case 0 :
		Instance->Montage_JumpToSection("MeleeAttack_B");
		break;
	case 1 : 
		Instance->Montage_JumpToSection("MeleeAttack_C");
		break;
	case 2 : 
		Instance->Montage_JumpToSection("MeleeAttack_D");
		break;
	default:
		Instance->Montage_JumpToSection("MeleeAttack_A");
		break;
	}*/
}

void UMyAction_MeleeAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}





