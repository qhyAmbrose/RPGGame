// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "MyActionComponent.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"
#include "MyWorldUserWidget.h"
#include "AI/MyAIController.h"
#include "AI/NavigationSystemBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"


class UMyWorldUserWidget;
class AAIController;

AMyAICharacter::AMyAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<UMyAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UMyActionComponent>("ActionComp");

	// Ensures we receive a controlled when spawned in the level by our gamemode
	//Pawn is automatically possessed by an AI Controller whenever it is created.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Enabled on mesh to react to incoming projectiles,生成重叠事件
	GetMesh()->SetGenerateOverlapEvents(true);

	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";

	bIsNotHit=true;
}


void AMyAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//每当AI的视觉（锥形范围）看到Pawn时，调用绑定的函数OnPawnSeen()
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AMyAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &AMyAICharacter::OnHealthChanged);
}

void AMyAICharacter::OnPawnSeen(APawn* Pawn)
{
	/*FString ProjRotationMsg=FString::Printf(TEXT("I See U!:"));
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);*/

	// Ignore if target already set，如果不在视线之内，在蓝图中会赋给Target Actor
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		MulticastPawnSeen();
	}
	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 0.5f, true);
}

void AMyAICharacter::OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		//AI受到攻击时显示血条
		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<UMyWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}

		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		
		// Died
		if (NewHealth <= 0.0f)
		{
			// stop BT
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			// ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// set lifespan
			SetLifeSpan(10.0f);
		}
		
		//受到攻击僵直，设置受击动画的bool值，防止燃烧也僵直
		if(Delta<-10.f)
		{
			bIsNotHit=false;
			GetCharacterMovement()->StopMovementImmediately();
			GetWorldTimerManager().SetTimer(TimerHandle_GetHitDelay, this, &AMyAICharacter::AfterGetHit, 0.5f, false);
		}
	}
}

void AMyAICharacter::AfterGetHit()
{
	GetCharacterMovement()->ResetMoveState();
	bIsNotHit=true;
}


void AMyAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
	}
}


AActor* AMyAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}




void AMyAICharacter::MulticastPawnSeen_Implementation()
{
	UMyWorldUserWidget* NewWidget = CreateWidget<UMyWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	if (NewWidget)
	{
		NewWidget->AttachedActor = this;
		// Index of 10 (or anything higher than default of 0) places this on top of any other widget.
		// May end up behind the minion health bar otherwise.
		NewWidget->AddToViewport(10);
	}
}
