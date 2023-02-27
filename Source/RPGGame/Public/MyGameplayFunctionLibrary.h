// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
//一个全局的静态函数类，即不需要指定继承自某个C++类，这个全局类可以被当前工程下的所有蓝图类调用
class RPGGAME_API UMyGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:
    
    	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount);
    
    	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

		UFUNCTION(BlueprintCallable, Category = "Gameplay")
		static float GetHitReactionAngle(AActor* TargetActor, const FHitResult& HitResult);

};
