#pragma once


#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MyAnimNotifyState_MeleeAttack.generated.h"

UCLASS()
class RPGGAME_API UMyAnimNotifyState_MeleeAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	TArray<FHitResult>HitResult;//保存命中结果//

	TArray<AActor*>HitActors;//保存命中的Actor//

	AController*EventInstigator;//ApplyDamage函数参数//

	TSubclassOf<UDamageType>DamageTypeClass;//ApplyDamage函数参数//

	FVector LastLocation1;//插槽1位置//

	FVector LastLocation2;//插槽2位置//

	AMyCharacter* Player;//当前玩家//

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	USkeletalMeshComponent* Weapon;//玩家武器//

	TArray<AActor*> ActorsToIgnore;//射线忽略目标//

	//自带函数重写//
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)override;//Tick//

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)override;//通知结束//

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)override;//通知开始//
};
