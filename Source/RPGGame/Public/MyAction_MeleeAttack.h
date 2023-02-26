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

	/*UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	UPROPERTY(EditAnywhere, Category = "Attack")
	USoundBase* CastingSound;

	int32 Index;
	TArray<FString> StringTArray;
	FName SectionName;
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName RightHandSocketName;*/

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	
	UMyAction_MeleeAttack();
};
