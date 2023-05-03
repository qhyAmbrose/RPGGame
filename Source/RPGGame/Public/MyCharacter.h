 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActionComponent.h"
#include "MyAttributeComponent.h"
#include "MyInteractionComponent.h"
#include "MyWorldUserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMyWorldUserWidget;

UCLASS()
class RPGGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	UMyInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMyAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMyActionComponent* ActionComp;

	UMyWorldUserWidget* ActiveHealthBar;
	
	/*UPROPERTY(EditDefaultsOnly,Category="UI")
	TSubclassOf<UMyWorldUserWidget> HealthBarWidget;*/
	

	void MoveForward(float Value);

	void MoveRight(float Value);

	void SprintStart();

	void SprintStop();

	void PrimaryAttack();

	void BlackHoleAttack();

	void Skill_01Start();

	void Skill_01Stop();

	void Dash();

	void PrimaryInteract();

	void MeleeAttack01Begin();

	void RemoteAttackStart();
	void RemoteAttackStop();
	void AfterRemoteAttack();
	
	void GetInput();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void AttackCheck();
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	UUserWidget* MainHUD;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* GetHitAnim;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* DieMontage;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* RemoteAttackAnim;
	bool bAttacking;
	TArray<FString> StringTArray;
	int32 Index;
	FName SectionName;
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName RightHandSocketName;


	
	//该类可以被编写为摄像机的振荡振动或动画振动
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;
	
	UPROPERTY(EditDefaultsOnly,Category="Effects")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactSound;
	
	UFUNCTION()
    void OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Attack")
	float HitReactionAngle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsRemoteAttacking;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsSprinting;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsGetHit;
	AMyCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealthChange(float Amount = 100);

	UFUNCTION()
	float GetHitReactionAngle();
	FTimerHandle TimerHandle_RemoteAttackDelay;

};
