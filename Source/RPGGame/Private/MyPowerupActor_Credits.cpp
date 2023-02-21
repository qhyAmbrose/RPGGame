// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPowerupActor_Credits.h"
#include "MyPlayerState.h"



AMyPowerupActor_Credits::AMyPowerupActor_Credits()
{
	CreditsAmount = 80;
}


void AMyPowerupActor_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	if (AMyPlayerState* PS = InstigatorPawn->GetPlayerState<AMyPlayerState>())
	{
		PS->AddCredits(CreditsAmount);
		HideAndCooldownPowerup();
	}
}

