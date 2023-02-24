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

	void MeleeAttack01();
	
	
	void AttackBegin();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;
	bool bAttacking;
	TArray<FString> StringTArray;

	FName SectionName;
	int32 Index;

	
	UFUNCTION()
    void OnHealthChanged(AActor* InstigatorActor, UMyAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

public:
	AMyCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealthChange(float Amount = 100);
	
};
