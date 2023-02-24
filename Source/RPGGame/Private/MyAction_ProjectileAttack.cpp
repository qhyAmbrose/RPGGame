// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"

UMyAction_ProjectileAttack::UMyAction_ProjectileAttack()
{
	HandSocketName = "hand_r";
	AttackAnimDelay = 0.2f;
	YawOffset = 5.0f;
	ProjectileNum=1;
}


void UMyAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());

		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			//在AttackAnimDelay秒后，执行委托Delegate，不循环
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

//具体角度位置生成Projectile
void UMyAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		//①得到SpawnParams
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		//②得到SpawnTM
		//  计算ProjRotation
		
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		//端点距离观察距离较远（不太远，在未命中时仍朝十字准线方向调整）
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);
		FHitResult Hit;
		// returns true if we got to a blocking hit
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
		{
			// Overwrite trace end with impact point in world
			TraceEnd = Hit.ImpactPoint;
		}
		// 发射角度。find new direction/rotation from Hand pointing to impact point in world.
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
		
		/*FString ProjRotationMsg = FString::Printf(TEXT("ProjRotationMsg: %s"), *ProjRotation.ToString());
		GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);
		*/
		
		uint8 num=0;
		uint8 ProNum=ProjectileNum;
		float InitYaw=ProjRotation.Yaw;
		if(ProNum%2!=0)
		{
			GetWorld()->SpawnActor<AActor>(ProjectileClass,HandLocation,ProjRotation,SpawnParams);
			ProNum--;
		}
		while(ProNum>=2)
		{
			num++;
			ProjRotation.Yaw=InitYaw+YawOffset*num;
			GetWorld()->SpawnActor<AActor>(ProjectileClass,HandLocation,ProjRotation,SpawnParams);
			ProjRotation.Yaw=InitYaw-YawOffset*num;
			GetWorld()->SpawnActor<AActor>(ProjectileClass,HandLocation,ProjRotation,SpawnParams);
			ProNum-=2;
		}
	}

	StopAction(InstigatorCharacter);
}

