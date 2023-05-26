// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyDelegateActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnDelegateChanged,UMyDelegateActorComponent*,DelegateComp,float, Rage, float, DamageAmount, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGGAME_API UMyDelegateActorComponent : public UActorComponent
{
	GENERATED_BODY()

public: 

	// 为该组件的属性设置默认值
	UMyDelegateActorComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDelegateChanged OnRageChanged;

protected:

	// 游戏开始时调用
	UFUNCTION(BlueprintCallable)

	void HandleTakeDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxRage;

public: 

	// 每一帧都调用
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
		

