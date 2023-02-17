// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActionEffect.h"
#include "MyAttributeComponent.h"
#include "MyActionEffect_Thorns.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UMyActionEffect_Thorns : public UMyActionEffect
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Thorns")
	float ReflectFraction;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta);

public:

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

	UMyActionEffect_Thorns();
};
