// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/MyAICharacter.h"
#include "MyAttributeComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "MyCharacter.h"
#include "MyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "../RPGGame.h"
#include "MyActionComponent.h"
#include "Engine/AssetManager.h"


//控制台变量
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

AMyGameModeBase::AMyGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 20;
	CooldownTimeBetweenFailures = 8.0f;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	PlayerStateClass = AMyPlayerState::StaticClass();

	SlotName = "SaveGame01";
}

void AMyGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AMyGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Run EQS to find potential power-up spawn locations
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AMyGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}

void AMyGameModeBase::KillAll()
{
	for (TActorIterator<AMyAICharacter> It(GetWorld()); It; ++It)
	{
		AMyAICharacter* Bot = *It;

		UMyAttributeComponent* AttributeComp = UMyAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}


void AMyGameModeBase::SpawnBotTimerElapsed()
{
	//在GameThread中查找该控制台变量的值（默认为true），若为false则跳出该生成机器人的函数
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		return;
	}

	// Give points to spend
	if (SpawnCreditCurve)
	{
		AvailableSpawnCredit += SpawnCreditCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (CooldownBotSpawnUntil > GetWorld()->TimeSeconds)
	{
		// Still cooling down
		return;
	}

	//LogOnScreen(this, FString::Printf(TEXT("Available SpawnCredits: %f"), AvailableSpawnCredit));

	// Count alive bots before spawning
	int32 NrOfAliveBots = 0;
	for (TActorIterator<AMyAICharacter> It(GetWorld()); It; ++It)
	{
		AMyAICharacter* Bot = *It;

		UMyAttributeComponent* AttributeComp = UMyAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	const float MaxBotCount = 40.0f;
	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	if (MonsterTable)
	{
		// Reset before selecting new row
		SelectedMonsterRow = nullptr;

		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);

		// Get total weight
		float TotalWeight = 0;
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;
		}

		// Random number within total random
		int32 RandomWeight = FMath::RandRange(0.0f, TotalWeight);

		//Reset
		TotalWeight = 0;

		// Get monster based on random weight
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;

			if (RandomWeight <= TotalWeight)
			{
				SelectedMonsterRow = Entry;
				break;
			}
		}

		if (SelectedMonsterRow && SelectedMonsterRow->SpawnCost >= AvailableSpawnCredit)
		{
			// Too expensive to spawn, try again soon
			CooldownBotSpawnUntil = GetWorld()->TimeSeconds + CooldownTimeBetweenFailures;

			//LogOnScreen(this, FString::Printf(TEXT("Cooling down until: %f"), CooldownBotSpawnUntil), FColor::Red);
			return;
		}
	}

	// Run EQS to find valid spawn location
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AMyGameModeBase::OnBotSpawnQueryCompleted);
	}
}


void AMyGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	/*if (Locations.IsValidIndex(0) && MonsterTable)
	{	
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			// Apply spawn cost
			AvailableSpawnCredit -= SelectedMonsterRow->SpawnCost;

			FPrimaryAssetId MonsterId = SelectedMonsterRow->MonsterId;

			TArray<FName> Bundles;
			FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AMyGameModeBase::OnMonsterLoaded, MonsterId, Locations[0]);
			Manager->LoadPrimaryAsset(MonsterId, Bundles, Delegate);
		}	
	}*/
	if (Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass,Locations[0],FRotator::ZeroRotator);
	}
}


void AMyGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep used locations to easily check distance between points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	// Break out if we reached the desired count or if we have no more potential positions remaining
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Pick a random location from remaining points.
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check minimum distance requirement
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if (!bValidLocation)
		{
			continue;
		}

		// Pick a random powerup-class
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// Keep for distance checks
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}


void AMyGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

//Actor被杀事件
void AMyGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	// Handle Player death
	AMyCharacter* Player = Cast<AMyCharacter>(VictimActor);
	if (Player)
	{
		// Disabled auto-respawn
// 		FTimerHandle TimerHandle_RespawnDelay;
// 		FTimerDelegate Delegate;
// 		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());
// 
// 		float RespawnDelay = 2.0f;
// 		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);

		// Store time if it was better than previous record
		AMyPlayerState* PS = Player->GetPlayerState<AMyPlayerState>();
		if (PS)
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}

		// Immediately auto save on death
		//WriteSaveGame();
	}

	// Give Credits for kill
	APawn* KillerPawn = Cast<APawn>(Killer);
	// Don't credit kills of self
	if (KillerPawn && KillerPawn != VictimActor)
	{
		// Only Players will have a 'PlayerState' instance, bots have nullptr
		AMyPlayerState* PS = KillerPawn->GetPlayerState<AMyPlayerState>();
		if (PS) 
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}



