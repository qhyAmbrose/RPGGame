// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMagicProjectile.h"

#include "MyActionComponent.h"
#include "MyBlueprintFunctionLibrary.h"

AMyMagicProjectile::AMyMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMyMagicProjectile::OnActorOverlap);

	InitialLifeSpan = 10.0f;

	DamageAmount = 20.0f;
}

//OtherActor为被Projectile打到的Actor
void AMyMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		//得到OtherActor的ActionComponent
		UMyActionComponent* ActionComp = Cast<UMyActionComponent>(OtherActor->GetComponentByClass(UMyActionComponent::StaticClass()));
		//Apply Damage & Impulse
		if (UMyBlueprintFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();
		}
	}
}
