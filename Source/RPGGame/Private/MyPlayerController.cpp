// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"





void AMyPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, false);
		}

		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, true);
		}
	}
}


void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &AMyPlayerController::TogglePauseMenu);
}


void AMyPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}


void AMyPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}


void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}
