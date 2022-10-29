// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectileBase.h"
#include "MyMagicProjectile.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API AMyMagicProjectile : public AMyProjectileBase
{
	GENERATED_BODY()
public:
	AMyMagicProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	/*UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;*/

	
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere,Category="Damage")
	FName SocketName;

};
