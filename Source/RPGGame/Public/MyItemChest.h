// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGamePlayInterface.h"
#include "GameFramework/Actor.h"
#include "MyItemChest.generated.h"

UCLASS()
class RPGGAME_API AMyItemChest : public AActor,public IMyGamePlayInterface
{
	GENERATED_BODY()
	
public:	
	AMyItemChest();

	void Interact_Implementation(APawn* InstigatorPawn) override;
protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BottomMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(EditAnywhere)
	float TargetPitch;

	UPROPERTY(BlueprintReadOnly)
	bool bLidOpened;
};
