// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMagicProjectile.h"

#include "EngineUtils.h"
#include "MyAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "MyGameplayFunctionLibrary.h"
#include "MyActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MyActionEffect.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectileBase.h"


AMyMagicProjectile::AMyMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMyMagicProjectile::OnActorOverlap);
	
	//How long this Actor lives before dying
	InitialLifeSpan = 10.0f;

	DamageAmount = 20.0f;

	SocketName="head";
	
}

void AMyMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	/*FTimerHandle TimerHandle_FindMinion;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "FindMinionLocation");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FindMinion, Delegate, 0.01, false);*/
}


//OtherActor为被Projectile打到的Actor
void AMyMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		//static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Status.Parrying");
		
		//得到OtherActor的ActionComponent
		UMyActionComponent* ActionComp = Cast<UMyActionComponent>(OtherActor->GetComponentByClass(UMyActionComponent::StaticClass()));
		
		//反弹效果实现
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			//当Projectile射中OtherActor时，让Projectile反弹回来
			MovementComp->Velocity = -MovementComp->Velocity;
			//Projectile反弹时，此时射出者变为OtherActor，所以将OtherActor设为Instigator
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		// Apply Damage & Impulse
		if (UMyGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			if (ActionComp && BurningActionClass && HasAuthority())
			{
				//给怪物附加BurningAction
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}

//执行追踪算法
void AMyMagicProjectile::FindMinionLocation()
{
	
	FVector StartLocation=this->GetActorLocation();
	/*FString msg = FString::Printf(TEXT("#########:%s"),*StartLocation.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Red, msg);*/
	FVector TargetLocation;
	float MinLocation=99999999999;
	for(TActorIterator<ACharacter> ActorItr(GetWorld());ActorItr;++ActorItr)
	{
		if (ActorItr->GetName().Contains("MinionRangedBP"))
		{
			ACharacter* AIMinion = Cast<ACharacter>(*ActorItr);
			/*FString msg = FString::Printf(TEXT("#########:%s"), *ActorItr->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, msg);*/

			FVector MinionLocation=AIMinion->GetMesh()->GetSocketLocation(SocketName);
			FVector Distance=MinionLocation-StartLocation;
			if(Distance.SizeSquared()<MinLocation)
			{
				MinLocation=Distance.SizeSquared();
				TargetLocation=Distance;
			}
		}
	}
	FRotator ProjRotation=FRotationMatrix::MakeFromX(StartLocation - TargetLocation).Rotator();
	MovementComp->Velocity=-ProjRotation.Vector()*800;
}


