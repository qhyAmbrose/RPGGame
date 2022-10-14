// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
 	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	CameraComp=CreateDefaultSubobject<UCameraComponent>("CameraComp");
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	//能够鼠标上下转动玩家视角
	CameraComp->bUsePawnControlRotation=true;
	
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	
}

void AMyCharacter::MoveForward(float Value)
{
	//Get the rotation of the Controller, often the 'view' rotation of this Pawn.
	FRotator ControlRotation=GetControlRotation();
	
	//前后左右移动的时候只需要水平旋转的角度也就是绕Z轴的Yaw，其他方向的角度为0即可
	ControlRotation.Pitch=0.0f;
	ControlRotation.Roll=0.0f;
	
	//Convert a rotation into a unit vector facing in its direction.
	AddMovementInput(ControlRotation.Vector(),Value);
}

void AMyCharacter::MoveRight(float Value)
{
	FRotator ControlRotation=GetControlRotation();
	ControlRotation.Pitch=0.0f;
	ControlRotation.Roll=0.0f;
	
	//旋转矩阵是一个四维向量矩阵，然后得到该旋转矩阵在Y轴方向上的向量
	FVector RightVector=FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector,Value);
}


