// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActionEffect.h"
#include "MyActionComponent.h"
#include "GameFramework/GameStateBase.h"




UMyActionEffect::UMyActionEffect()
{
	bAutoStart = true;
}


void UMyActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		//持续时间结束后调用StopAction
		GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);

		//每Period调用一次
		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, Delegate, Period, true);
	}
}


void UMyActionEffect::StopAction_Implementation(AActor* Instigator)
{
	//因为可能Duration（=3）可能比Period（=1）先触发，当3秒时执行StopAction时，Period还没有但即将触发第三次，此时判断一下PeriodHandle的剩余时间是否小于一个极小的数(1.e-4f) (0.00001)
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);

	UMyActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->RemoveAction(this);
	}
}


float UMyActionEffect::GetTimeRemaining() const
{
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (GS)
	{
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}

	return Duration;
}


void UMyActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{

}
