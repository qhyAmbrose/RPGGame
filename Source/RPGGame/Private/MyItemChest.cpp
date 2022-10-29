// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemChest.h"

// Sets default values
AMyItemChest::AMyItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BottomMesh=CreateDefaultSubobject<UStaticMeshComponent>("BottomMesh");
	BottomMesh->SetupAttachment(RootComponent);
	LidMesh=CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BottomMesh);

	TargetPitch=120.0f;
}

void AMyItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	//互动键更改宝箱的状态
	bLidOpened=!bLidOpened;
	//开关宝箱（更改宝箱盖子旋转）
	float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}



