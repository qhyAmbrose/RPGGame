// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAction.h"
#include "MyAction_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UMyAction_MeleeAttack : public UMyAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName HandSocketName;
 
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	UPROPERTY(EditAnywhere, Category = "Attack")
	USoundBase* CastingSound;

	UFUNCTION()
	virtual void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);
public:

	virtual void StartAction_Implementation(AActor* Instigator) override;
	
	UMyAction_MeleeAttack();
};
