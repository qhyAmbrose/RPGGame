// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAttributeComponent.h"
#include "MyGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


UMyAttributeComponent::UMyAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;

	SetIsReplicatedByDefault(true);
}


bool UMyAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}


bool UMyAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool UMyAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}


float UMyAttributeComponent::GetHealth() const
{
	return Health;
}

float UMyAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}


bool UMyAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = NewHealth - OldHealth;

	// Is Server?只有Server才能调用的功能
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (ActualDelta != 0.0f)
		{
			//在Server和所有Clients上广播（主要是处理死亡以及其他状态（血条的显示）的改变，并不会广播血量的数值）
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// Died
		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			AMyGameModeBase* GM = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	//如果死亡，返回true
	return ActualDelta != 0;
}




UMyAttributeComponent* UMyAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UMyAttributeComponent>(FromActor->GetComponentByClass(UMyAttributeComponent::StaticClass()));
	}

	return nullptr;
}


bool UMyAttributeComponent::IsActorAlive(AActor* Actor)
{
	UMyAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}


void UMyAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}


