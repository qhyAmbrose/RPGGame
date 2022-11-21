// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

#include "Net/UnrealNetwork.h"

void AMyPlayerState::AddCredits(int32 Delta)
{
	// Avoid user-error of adding a negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}


bool AMyPlayerState::RemoveCredits(int32 Delta)
{
	// Avoid user-error of adding a subtracting negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}

	if (Credits < Delta)
	{
		// Not enough credits available
		return false;
	}

	Credits -= Delta;

	OnCreditsChanged.Broadcast(this, Credits, -Delta);

	return true;
}


bool AMyPlayerState::UpdatePersonalRecord(float NewTime)
{
	// Higher time is better
	if (NewTime > PersonalRecordTime)
	{
		float OldRecord = PersonalRecordTime;

		PersonalRecordTime = NewTime;

		OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, OldRecord);

		return true;
	}

	return false;
}




void AMyPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}





// void AMyPlayerState::MulticastCredits_Implementation(float NewCredits, float Delta)
// {
// 	OnCreditsChanged.Broadcast(this, NewCredits, Delta);
// }


int32 AMyPlayerState::GetCredits() const
{
	return Credits;
}


void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, Credits);
}