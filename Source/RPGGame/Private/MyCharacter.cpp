// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
 	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	CameraComp=CreateDefaultSubobject<UCameraComponent>("CameraComp");
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp=CreateDefaultSubobject<UMyInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<UMyAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UMyActionComponent>("ActionComp");
	//能够鼠标上下转动玩家视角
	CameraComp->bUsePawnControlRotation=true;

	TimeToHitParamName = "TimeToHit";
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryInteract",IE_Pressed,this,&AMyCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("PrimaryAttack",IE_Pressed,this,&AMyCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack",IE_Pressed,this,&AMyCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Skill01",IE_Pressed,this,&AMyCharacter::Skill_01Start);
	PlayerInputComponent->BindAction("Skill01",IE_Released,this,&AMyCharacter::Skill_01Stop);

	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&AMyCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&AMyCharacter::SprintStop);
	PlayerInputComponent->BindAction("Dash",IE_Pressed,this,&AMyCharacter::Dash);

	PlayerInputComponent->BindAction("MeleeAttack01",IE_Pressed,this,&AMyCharacter::MeleeAttack01_OnClickBegin);
	PlayerInputComponent->BindAction("MeleeAttack01",IE_Released,this,&AMyCharacter::MeleeAttack01_OnClickEnd);
	
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

void AMyCharacter::SprintStart()
{
	ActionComp->StartActionByName(this,"Sprint");
}

void AMyCharacter::SprintStop()
{
	ActionComp->StopActionByName(this,"Sprint");
}

void AMyCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void AMyCharacter::BlackHoleAttack()
{
	ActionComp->StartActionByName(this,"Blackhole");
}

void AMyCharacter::Skill_01Start()
{
	ActionComp->StartActionByName(this,"Skill01");
}

void AMyCharacter::Skill_01Stop()
{
	ActionComp->StopActionByName(this,"Skill01");
}

void AMyCharacter::Dash()
{
	ActionComp->StartActionByName(this,"Dash");
}

void AMyCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void AMyCharacter::MeleeAttack01()
{
	ActionComp->StartActionByName(this,"MeleeAttack01");
}

void AMyCharacter::MeleeAttack01_OnClickBegin()
{
	bClicking = true;
	if (!bAttacking ) {
		AttackBegin();
	}
}

void AMyCharacter::MeleeAttack01_OnClickEnd()
{
	bClicking = false;
}

void AMyCharacter::AttackBegin()
{
	bAttacking = true;
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	if (AttackAnim && Instance && !Instance->Montage_IsPlaying(AttackAnim)) {
		Instance->Montage_Play(AttackAnim);

		switch (FMath::RandRange((int32)1, 4)) {
		case 1 : 
			Instance->Montage_JumpToSection(FName("MeleeAttack_A"), AttackAnim);
			break;
		case 2 : 
			Instance->Montage_JumpToSection(FName("MeleeAttack_B"), AttackAnim);
			break;
		case 3 : 
			Instance->Montage_JumpToSection(FName("MeleeAttack_C"), AttackAnim);
			break;
		case 4 :
			Instance->Montage_JumpToSection(FName("MeleeAttack_D"),AttackAnim);
		}
	}
}

void AMyCharacter::AttackEnd()
{
	bAttacking = false;
	// 鼠标还是按着的状态，那么就继续攻击
	if (bClicking ) { 
		AttackBegin();
	}
}

//对玩家造成伤害/治愈
void AMyCharacter::HealthChange(float Amount)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

//应用伤害/治愈的最后一步
void AMyCharacter::OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth,float Delta)
{
	// Damaged
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		//防止重复生成
		/*if(ActiveHealthBar==nullptr)
		{
			FString ProjRotationMsg=FString::Printf(TEXT("ProjRotationMsg:"));
			GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);
			
			//生成血条组件并显示
			ActiveHealthBar=CreateWidget<UMyWorldUserWidget>(GetWorld(),HealthBarWidget);
			if(ActiveHealthBar)
			{
				//AttachedActor赋给被打到的人物指针
				ActiveHealthBar->AttachedActor=this;
				ActiveHealthBar->AddToViewport();
			}
		}*/
	}

	// Died
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
	}
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//用于将UObject实例和成员UFUNCTION绑定到动态多播委托的帮助器宏。
	//参数：
	//UserObject–UObject实例
	//FuncName–指向成员UFUNCTION的函数指针，通常形式为&UClassName::FunctionName
	//OnHealthChanged是一个委托变量
	AttributeComp->OnHealthChanged.AddDynamic(this, &AMyCharacter::OnHealthChanged);
}

FVector AMyCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}



