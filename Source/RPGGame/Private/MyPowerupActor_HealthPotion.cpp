// Fill out your copyright notice in the Description page of Project Settings.
//实现了血包满血，但没法实现扣除积分功能

#include "MyPowerupActor_HealthPotion.h"
#include "MyAttributeComponent.h"
#include "MyPlayerState.h"


#define LOCTEXT_NAMESPACE "InteractableActors"



AMyPowerupActor_HealthPotion::AMyPowerupActor_HealthPotion()
{
	CreditCost = 50;
}


void AMyPowerupActor_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UMyAttributeComponent* AttributeComp = UMyAttributeComponent::GetAttributes(InstigatorPawn);
	// Check if not already at max health
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		//加满血
		/*if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
		{
			// Only activate if healed successfully
			HideAndCooldownPowerup();
		}*/
		if (AMyPlayerState* PS = InstigatorPawn->GetPlayerState<AMyPlayerState>())
		{
			if (PS->RemoveCredits(CreditCost) && AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
			{
				// Only activate if healed successfully
				HideAndCooldownPowerup();
			}
		}
	}
}

FText AMyPowerupActor_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	UMyAttributeComponent* AttributeComp = UMyAttributeComponent::GetAttributes(InstigatorPawn);
	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCost);
}


#undef LOCTEXT_NAMESPACE