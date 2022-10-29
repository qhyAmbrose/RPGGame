// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractionComponent.h"

#include "MyGamePlayInterface.h"

// Sets default values for this component's properties
UMyInteractionComponent::UMyInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}


// Called when the game starts
void UMyInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyInteractionComponent::FindInteractTarget()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	
	TArray<FHitResult> Hits;

	GetWorld()->SweepMultiByObjectType(Hits,EyeLocation,End , FQuat::Identity,ObjectQueryParams , Shape);

	for(FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if(HitActor)
		{
			TargetActor=HitActor;
		}
	}
}

void UMyInteractionComponent::PrimaryInteract()
{
	if(TargetActor)
	{
		APawn* MyPawn = Cast<APawn>(GetOwner());
		//传入交互目标与交互执行者
		IMyGamePlayInterface::Execute_Interact(TargetActor,MyPawn);
	}
}


// Called every frame
void UMyInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//找到互动物体
	FindInteractTarget();
}

