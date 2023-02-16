// Fill out your copyright notice in the Description page of Project Settings.
//流星火雨技能实现

#include "MyAction_Skill01.h"

#include "MyCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UMyAction_Skill01::UMyAction_Skill01()
{
	HandSocketName = "head";
	DecalDelay = 0.01f;
	DecalLifeSpan = 0.01f;
	Skill01Delay=0.1f;
	DecalOffsetZ=50.0f;
	MinOffset=-50;
	MaxOffset=50;
	Skill01LifeSpan=3.0f;
}

void UMyAction_Skill01::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(BeginAttackAnim);
		
		if (Character->HasAuthority())
		{
			FTimerDelegate Delegate;

			Delegate.BindUFunction(this, "DecalDelay_Elapsed", Character);

			//在DecalDelay秒后，执行委托Delegate
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, DecalDelay, true);
		}
	}
}

//松开技能键以释放技能
void UMyAction_Skill01::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
	ACharacter* Character = Cast<ACharacter>(Instigator);
	if(Character)
	{
		Character->PlayAnimMontage(EndAttackAnim);
		//在玩家的手上显示该粒子特效
		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//播放声音
		UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());
		//停止技能指示器显示
		if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_AttackDelay))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AttackDelay);
		}

		//生成贴图
		FTimerDelegate DecalDelegate;
		DecalDelegate.BindUFunction(this, "Skill01Decal", Character,GetDecalPosition(Character));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DecalDelay,DecalDelegate, DecalDelay, true);

		//生成技能
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "Skill01Delay_Elapsed", Character,GetDecalPosition(Character));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Skill01Delay, Delegate, Skill01Delay,true,0.5f);

		//终止技能
		FTimerDelegate StopDelegate;
		StopDelegate.BindUFunction(this, "Skill01Stop", Character);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Skill01Stop,StopDelegate,1.0f,false,Skill01LifeSpan+0.5f);
		
	}
}

//生成技能指示器
void UMyAction_Skill01::DecalDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	FVector DecalLocation=GetDecalPosition(InstigatorCharacter);

	
	FRotator DecalRotation=FRotator::ZeroRotator;
	DecalRotation.Pitch=-90;
	UGameplayStatics::SpawnDecalAtLocation(this,DecalMaterial, DecalSize, DecalLocation, DecalRotation,DecalLifeSpan);
	//GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,DecalLocation.ToString());
}

//先在技能指示器上生成魔法阵特效
void UMyAction_Skill01::Skill01Decal(ACharacter* InstigatorCharacter,FVector ProjLocation)
{
	FRotator ProjRotation=FRotator::ZeroRotator;
	ProjRotation.Pitch=-90;
	
	UGameplayStatics::SpawnDecalAtLocation(this,Skill01Material, DecalSize, ProjLocation, ProjRotation,DecalLifeSpan);
}

//实现流星火雨技能
void UMyAction_Skill01::Skill01Delay_Elapsed(ACharacter* InstigatorCharacter,FVector ProjLocation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;
	
	FRotator ProjRotation=FRotator::ZeroRotator;
	ProjRotation.Pitch=-90;
	
	//然后随机出现投射物向下发射
	ProjLocation.Z=ProjLocation.Z+DecalOffsetZ;
	float OffsetX=FMath::FRandRange(MinOffset,MaxOffset);
	float OffsetY=FMath::FRandRange(MinOffset,MaxOffset);
	float OffsetZ=FMath::FRandRange(MinOffset,MaxOffset);
	ProjLocation.X+=OffsetX;
	ProjLocation.Y+=OffsetY;
	ProjLocation.Z+=OffsetZ;

	GetWorld()->SpawnActor<AActor>(ProjectileClass,ProjLocation,ProjRotation,SpawnParams);
}
//终止skill_01
void UMyAction_Skill01::Skill01Stop(ACharacter* InstigatorCharacter)
{
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Skill01Delay))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Skill01Delay);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DecalDelay);

	}
}
//得到贴画当前的位置
FVector UMyAction_Skill01::GetDecalPosition(ACharacter* InstigatorCharacter)
{
	
	FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
	FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 50000);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FHitResult Hit;
	if(GetWorld()->LineTraceSingleByObjectType(Hit,TraceStart,TraceEnd,ObjParams))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	return TraceEnd;
}






