// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "MyTargetDummy.generated.h"

UCLASS()
class RPGGAME_API AMyTargetDummy : public AActor
{
	GENERATED_BODY()
public:	

	AMyTargetDummy();
protected:

	UPROPERTY(VisibleAnywhere)
	UMyAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta);

};
