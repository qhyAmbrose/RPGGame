#include "MyDelegateActorComponent.h"

// 为该组件的属性设置默认值
UMyDelegateActorComponent::UMyDelegateActorComponent()
{
	// 将此组件设置为在游戏开始时初始化，并且每帧更新函数。  如果你不需要这些功能，你可以把它们
	// 关闭以便提高性能。
	PrimaryComponentTick.bCanEverTick = true;
	MaxRage = 100.0f;
}

// 游戏开始时调用
void UMyDelegateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	//获取此Actor组件的所有者。
	AActor* MyOwner = GetOwner();

	if(MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UMyDelegateActorComponent::HandleTakeDamage);

		//所有者对象现在一定会响应OnTakeAnyDamage函数。        
		//MyOwner->OnTakeAnyDamage.AddDynamic(this,&UMyDelegateActorComponent::HandleTakeAnyDamage);
	}

	//将初始怒气值设为0。
	Rage = 0;
}

void UMyDelegateActorComponent::HandleTakeDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage <= 0.0f)
	{
		//伤害量为0或以下。
		return;
	} 

	Rage = FMath::Clamp(Rage - Damage, 0.0f, MaxRage);
	OnRageChanged.Broadcast(this, Rage, Damage, DamageType, InstigatedBy, DamageCauser);
}

// 每一帧都调用
void UMyDelegateActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}