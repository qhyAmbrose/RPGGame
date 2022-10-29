// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAction.h"
#include "MyAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UParticleSystem;
class USoundBase;
/**
 * 
 */
UCLASS()
class RPGGAME_API UMyAction_ProjectileAttack : public UMyAction
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Effects")
	FName HandSocketName;
 
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	/* Particle System played during attack animation */
	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	/* Sound Effect to play (Can be Wave or Cue) */
	UPROPERTY(EditAnywhere, Category = "Attack")
	USoundBase* CastingSound;

	//调整投射物生成角度
	UPROPERTY(EditAnywhere,Category="Attack")
	float YawOffset;

	UPROPERTY(EditAnywhere,Category="Attack")
	uint8 ProjectileNum;

	UFUNCTION()
	virtual void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	UMyAction_ProjectileAttack();
};
