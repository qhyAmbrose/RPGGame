// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "MyGameplayFunctionLibrary.h"
#include "AI/MyAICharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


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

	StringTArray.Add("MeleeAttack_A");
	StringTArray.Add("MeleeAttack_B");
	StringTArray.Add("MeleeAttack_C");
	StringTArray.Add("MeleeAttack_D");
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

	PlayerInputComponent->BindAction("MeleeAttack01",IE_Pressed,this,&AMyCharacter::MeleeAttack01Begin);
	
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

void AMyCharacter::MeleeAttack01Begin()
{
	//主要是完成标签的工作
	ActionComp->StartActionByName(this,"MeleeAttack01");
	
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	//如果收招则从第一招开始打，并设置为连击状态
	if(!bAttacking)
	{
		Instance->Montage_Play(AttackAnim);
		bAttacking=true;
	}
	//得到当前蒙太奇片段的名称，与构造函数中声明的数组一一对应
	SectionName=Instance->Montage_GetCurrentSection(AttackAnim);
	FString TargetName(SectionName.ToString());
	StringTArray.Find(TargetName, Index);
	//GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,TargetName);

	//当攻击A的时候按攻击键跳向B，。。。
	switch (Index) {
		case 0 :
			Instance->Montage_JumpToSection("MeleeAttack_B");
			break;
		case 1 : 
			Instance->Montage_JumpToSection("MeleeAttack_C");
			break;
		case 2 : 
			Instance->Montage_JumpToSection("MeleeAttack_D");
			break;
		default:
			Instance->Montage_JumpToSection("MeleeAttack_A");
			break;
	}
}

//每个蒙太奇片段结束后的动画通知都会调用的函数
void AMyCharacter::AttackEnd()
{
	//收招并设置为非连击状态
	bAttacking = false;
	StopAnimMontage(AttackAnim);
	//移除标签
	ActionComp->StopActionByName(this,"MeleeAttack01");
}
//查询近战攻击目标并应用伤害
void AMyCharacter::AttackCheck()
{
		/*ActionComp->StartActionByName(this,"MeleeAttack01");*/
	FVector HandLocation;
	if(Index%2==0)
	{
		HandLocation = this->GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	else
	{
		HandLocation = this->GetMesh()->GetSocketLocation(RightHandSocketName);
	}

		//①得到SpawnParams
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart =HandLocation;
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		//端点距离观察距离较远（不太远，在未命中时仍朝十字准线方向调整）
		FVector TraceEnd = TraceStart + (this->GetControlRotation().Vector() * 20);
		FHitResult Hit;
		
		// returns true if we got to a blocking hit
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
		{
			GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,"Right!");
			//对击中目标进行伤害以及受击动画的操作
			AMyAICharacter* Monsters=Cast<AMyAICharacter>(Hit.GetActor());
			// Apply Damage & Impulse
			if (UMyGameplayFunctionLibrary::ApplyDirectionalDamage(this, Monsters, 80, Hit))
			{
				/*//呈现粒子效果
				UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

				//播放音效
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

				//摄像机镜头晃动
				UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);*/

				/*if (ActionComp && BurningActionClass && HasAuthority())
				{
					//给怪物附加BurningAction
					ActionComp->AddAction(GetInstigator(), BurningActionClass);
				}*/
			}
		}
		/*FString ProjRotationMsg = FString::Printf(TEXT("ProjRotationMsg: %s"), *ProjRotation.ToString());
		GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);
		*/
		DrawDebugSphere(GetWorld(),TraceEnd,20.f,8,FColor::Green,false,1);
		
			
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






