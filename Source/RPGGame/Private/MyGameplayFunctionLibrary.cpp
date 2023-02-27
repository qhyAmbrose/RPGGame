// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayFunctionLibrary.h"
#include "MyAttributeComponent.h"
#include "AI/MyAICharacter.h"



bool UMyGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	UMyAttributeComponent* AttributeComp = UMyAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}


bool UMyGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}

//计算攻击者与被攻击者的角度
float UMyGameplayFunctionLibrary::GetHitReactionAngle(AActor* TargetActor, const FHitResult& HitResult)
{
	//被打击对象的位置
	FVector TargetActorLocation=TargetActor->GetActorLocation();
	//打击点的位置
	FVector HitLocation=HitResult.Location;
	//让这两个向量在一个平面上
	TargetActorLocation.Z=HitLocation.Z;
	//计算两个向量之间的夹角
	/*float a;
	a=HitLocation.Dot(TargetActorLocation);
	float b=a/(HitLocation.Length()*TargetActorLocation.Length());
	float Angel=FMath::RadiansToDegrees(acosf(b));*/
	FRotator TargetRotation=FRotationMatrix::MakeFromX(TargetActorLocation-HitLocation).Rotator();
		
	float HitReactionAngle=TargetRotation.Yaw;
	if(HitReactionAngle<0)
	{
		HitReactionAngle+=360;
	}
	FString ProjRotationMsg=FString::Printf(TEXT("%f"),HitReactionAngle);
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);
	
	return HitReactionAngle;
}






