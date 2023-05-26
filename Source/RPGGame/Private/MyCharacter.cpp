// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "MyGameplayFunctionLibrary.h"
#include "TimerManager.h"
#include "AI/MyAICharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
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

	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	//子弹时间
	//this->CustomTimeDilation=0.1;
	bIsBlend=false;
	BlendTime=1.0f;
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
	PlayerInputComponent->BindAction("RemoteAttack",IE_Pressed,this,&AMyCharacter::RemoteAttackStart);
	PlayerInputComponent->BindAction("RemoteAttack",IE_Released,this,&AMyCharacter::RemoteAttackStop);
	
}

void AMyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//更改蓄力镜头的远近
	if(bIsBlend&&CameraComp->FieldOfView>70)
	{
		CameraComp->FieldOfView-=DeltaSeconds*BlendTime;
	}
	if(!bIsBlend&&CameraComp->FieldOfView<90)
	{
		CameraComp->FieldOfView+=DeltaSeconds*BlendTime*3;
	}
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
	bIsSprinting=true;
	//关闭摄像机延迟跟随
	SpringArmComp->bEnableCameraLag=false;
	if(CameraShake_Sprint)
	{
		CurrentSprintShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(CameraShake_Sprint,1);
	}
}

void AMyCharacter::SprintStop()
{
	ActionComp->StopActionByName(this,"Sprint");
	bIsSprinting=false;
	//开启摄像机延迟跟随
	SpringArmComp->bEnableCameraLag=true;
	if(CurrentSprintShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopCameraShake(CurrentSprintShake,true);
		CurrentSprintShake=nullptr;
	}
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
	//生成围绕玩家旋转的远程武器
	//GetWorld()->SpawnActor<AActor>(ProjectileClass,HandLocation,ProjRotation,SpawnParams);
	//主要是完成标签的工作
	ActionComp->StartActionByName(this,"MeleeAttack01");
}

void AMyCharacter::RemoteAttackStart()
{
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	Instance->Montage_Play(RemoteAttackAnim_A,0.5);
	//在手上播放粒子特效
	UGameplayStatics::SpawnEmitterAttached(CastingEffect, this->GetMesh(), RightHandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	
	UGameplayStatics::SpawnSoundAttached(CastingSound, this->GetMesh());

	//回调
	FOnMontageEnded EndMontageDelegate;
	EndMontageDelegate.BindUObject(this,&AMyCharacter::AfterRemoteAttack);
	Instance->Montage_SetEndDelegate(EndMontageDelegate);
	//用Timehandle做一个回调函数
	//GetWorldTimerManager().SetTimer(TimerHandle_RemoteAttackDelay_A,this,&AMyCharacter::AfterRemoteAttack,0.6f,false);
	
	GEngine->AddOnScreenDebugMessage(-1,2,FColor::Blue,"Start");
	/*ActionComp->StartActionByName(this,"RemoteAttack");
	bIsRemoteAttacking=true;*/
	
	bIsBlend=true;
	//摄像机镜头晃动
	//UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);
	//控制镜头焦距缓慢拉向越肩视角
	//CameraComp->FieldOfView=70.0f;
}
void AMyCharacter::AfterRemoteAttack(UAnimMontage* montage,bool bInterrupted)
{
	GEngine->AddOnScreenDebugMessage(-1,2,FColor::Black,"Callback");
	//设置移动速度以及转向速度
	//蒙太奇播放完后将蓄力完变量设置为true(在状态机中维持该endingpose）
	bIsRemoteAttacking=true;
	
}
void AMyCharacter::RemoteAttackStop()
{
	//结束粒子特效
	GEngine->AddOnScreenDebugMessage(-1,2,FColor::Black,"Stop");
	//播放第二段动画
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	Instance->Montage_Play(RemoteAttackAnim_B,1);
	//释放投射物
	//ActionComp->StartActionByName(this, "PrimaryAttack");
	//用Timehandle做一个回调函数
	
	GetWorldTimerManager().SetTimer(TimerHandle_RemoteAttackDelay_B,this,&AMyCharacter::RemoteAttackProjectile,0.43f,false);
	bIsBlend=false;
	//将是否蓄力完的变量设置为False
	bIsRemoteAttacking=false;
	/*ActionComp->StopActionByName(this,"RemoteAttack");
	bIsRemoteAttacking=false;*/

	//CameraComp->FieldOfView=90;
}



void AMyCharacter::RemoteAttackProjectile()
{
	FVector HandLocation = this->GetMesh()->GetSocketLocation(RightHandSocketName);

	//①得到SpawnParams
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	SpawnParams.Instigator = this;

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	// Ignore Player
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	//②得到SpawnTM
	//  计算ProjRotation
		
	FVector TraceStart = this->GetPawnViewLocation();
	// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
	//端点距离观察距离较远（不太远，在未命中时仍朝十字准线方向调整）
	FVector TraceEnd = TraceStart + (this->GetControlRotation().Vector() * 5000);
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
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Blue,ProjRotationMsg);*/
	GetWorld()->SpawnActor<AActor>(ProjectileClass,HandLocation,ProjRotation,SpawnParams);
	

	/*//摄像机镜头晃动
	UGameplayStatics::PlayWorldCameraShake(this, ImpactShake_Projectile, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);
	*/
	
}
//每个蒙太奇片段结束后的动画通知都会调用的函数
void AMyCharacter::AttackEnd()
{
	//移除标签
	ActionComp->StopActionByName(this,"MeleeAttack01");
	//收招并设置为非连击状态
	bAttacking = false;
	StopAnimMontage(AttackAnim);
	
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
		Shape.SetSphere(30);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart =HandLocation;
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		//端点距离观察距离较远（不太远，在未命中时仍朝十字准线方向调整）
		FVector TraceEnd = TraceStart + (this->GetControlRotation().Vector() * 50);
		FHitResult Hit;
		
		// returns true if we got to a blocking hit
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
		{
			//对击中目标进行伤害以及受击动画的操作
			AMyAICharacter* Monsters=Cast<AMyAICharacter>(Hit.GetActor());
			
			// Apply Damage & Impulse
			if (UMyGameplayFunctionLibrary::ApplyDirectionalDamage(this, Monsters, 15, Hit))
			{
				//得到受击角度
				HitReactionAngle=UMyGameplayFunctionLibrary::GetHitReactionAngle(Monsters,Hit);
				//呈现粒子效果
				UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

				//播放音效
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

				//摄像机镜头晃动
				UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

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
		/*DrawDebugSphere(GetWorld(),TraceEnd,30.f,8,FColor::Green,false,1);*/
		
			
}

//对玩家造成伤害/治愈
/*void AMyCharacter::HealthChange(float Amount)
{
	//GEngine->AddOnScreenDebugMessage(-1,2,FColor::Blue,"DynamicDecalre!!!!!!!!!!!");
	AttributeComp->ApplyHealthChange(this, Amount);
}*/
/*void AMyCharacter::GetInput()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	EnableInput(PC);
}*/
float AMyCharacter::GetHitReactionAngle()
{
	return HitReactionAngle;
}

//应用伤害/治愈的最后一步（动态多播，主要是该委托在AI中也同时生效）
void AMyCharacter::OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth,float Delta)
{
	//GEngine->AddOnScreenDebugMessage(-1,2,FColor::Blue,"DynamicDecalre!!!!!!!!!!!");
	// Damaged
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		/*//播放受击动画蒙太奇
		PlayAnimMontage(GetHitAnim);
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		FTimerHandle TimerHandle_GetHit;
		GetWorldTimerManager().SetTimer(TimerHandle_GetHit, this,&AMyCharacter::GetInput, 0.4f, false);*/
		
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

		PlayAnimMontage(DieMontage);
		SetLifeSpan(1.6f);
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

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UGameplayStatics::PlayWorldCameraShake(this,CameraShake_Land,GetActorLocation(),400,2000);
}






