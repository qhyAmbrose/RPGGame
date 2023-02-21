// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTargetDummy.h"
#include "Components/StaticMeshComponent.h"
#include "MyAttributeComponent.h"


AMyTargetDummy::AMyTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<UMyAttributeComponent>("AttributeComp");
	// Trigger when health is changed (damage/healing)
	AttributeComp->OnHealthChanged.AddDynamic(this, &AMyTargetDummy::OnHealthChanged);
}


void AMyTargetDummy::OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}
