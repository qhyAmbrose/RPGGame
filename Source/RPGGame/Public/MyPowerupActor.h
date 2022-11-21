// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGamePlayInterface.h"
#include "GameFramework/Actor.h"
#include "MyPowerupActor.generated.h"

class USphereComponent;
UCLASS()
class RPGGAME_API AMyPowerupActor : public AActor, public IMyGamePlayInterface
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive;

	UFUNCTION()
	void OnRep_IsActive();

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn);

public:

	AMyPowerupActor();


};
