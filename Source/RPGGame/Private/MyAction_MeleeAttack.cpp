// Fill out your copyright notice in the Description page of Project Settings.
#include "MyAction_MeleeAttack.h"
#include "MyCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UMyAction_MeleeAttack::UMyAction_MeleeAttack()
{
	HandSocketName = "hand_r";
	AttackAnimDelay = 0.2f;
}

void UMyAction_MeleeAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());

		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			//在AttackAnimDelay秒后，执行委托Delegate，不循环
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

//射线检测是否击中目标
void UMyAction_MeleeAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	
	StopAction(InstigatorCharacter);
}



