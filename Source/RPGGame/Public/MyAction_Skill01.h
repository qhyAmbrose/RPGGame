// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAction.h"
#include "MyAction_Skill01.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UMyAction_Skill01 : public UMyAction
{
	GENERATED_BODY()
	protected:

	UPROPERTY(EditAnywhere,Category="Skill01")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere,Category="Decal")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere,Category="Skill01")
	UMaterialInterface* Skill01Material;

	UPROPERTY(EditAnywhere,Category="Decal")
	FVector DecalSize;

	UPROPERTY(EditAnywhere,Category="Decal")
	float DecalLifeSpan;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float DecalDelay;

	UPROPERTY(EditAnywhere,Category="Skill01")
	float Skill01Delay;

	UPROPERTY(EditAnywhere,Category="Skill01")
	float DecalOffsetZ;

	UPROPERTY(EditAnywhere, Category = "Effects")
	FName HandSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Skill01")
	UAnimMontage* BeginAttackAnim;

	UPROPERTY(EditAnywhere,Category="Skill01")
	UAnimMontage* EndAttackAnim;

	UPROPERTY(EditAnywhere,Category="Skill01")
	float MinOffset;

	UPROPERTY(EditAnywhere,Category="Skill01")
	float MaxOffset;

	UPROPERTY(EditAnywhere,Category="Skill01")
	float Skill01LifeSpan;

	/* Particle System played during attack animation */
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* CastingEffect;

	/* Sound Effect to play (Can be Wave or Cue) */
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* CastingSound;

	UFUNCTION()
	void DecalDelay_Elapsed(ACharacter* InstigatorCharacter);

	UFUNCTION()
	void Skill01Decal(ACharacter* InstigatorCharacter,FVector ProjLocation);
	
	UFUNCTION()
	void Skill01Delay_Elapsed(ACharacter* InstigatorCharacter,FVector ProjLocation);

	UFUNCTION()
	void Skill01Stop(ACharacter* InstigatorCharacter);

	FTimerHandle TimerHandle_AttackDelay;
	FTimerHandle TimerHandle_DecalDelay;
	FTimerHandle TimerHandle_Skill01Delay;
	FTimerHandle TimerHandle_Skill01Stop;


public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	FVector GetDecalPosition(ACharacter* InstigatorCharacter);

	virtual  void StopAction_Implementation(AActor* Instigator) override;

	UMyAction_Skill01();
};
