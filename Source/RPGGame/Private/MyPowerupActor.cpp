// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPowerupActor.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AMyPowerupActor::AMyPowerupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	//该Actor重生的时间
	RespawnTime = 10.0f;
	bIsActive = true;

	SetReplicates(true);
}

void AMyPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// 在子类中编写
}


FText AMyPowerupActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}


void AMyPowerupActor::ShowPowerup()
{
	SetPowerupState(true);
}


void AMyPowerupActor::HideAndCooldownPowerup()
{
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AMyPowerupActor::ShowPowerup, RespawnTime);
}

void AMyPowerupActor::SetPowerupState(bool bNewIsActive)
{
	bIsActive = bNewIsActive;
	OnRep_IsActive();
}

void AMyPowerupActor::OnRep_IsActive()
{
	SetActorEnableCollision(bIsActive);
	// Set visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}

void AMyPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPowerupActor, bIsActive);
}

