// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAGameMode.h"
#include "HealthComponent.h"


AFFAGameMode::AFFAGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFFAGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*CheckAnyPlayerAlive();*/
}

void AFFAGameMode::CheckAnyPlayerAlive()
{
	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//{
	//	APlayerController* PC = It->Get();
	//	if (PC && PC->GetPawn())
	//	{
	//		APawn* MyPawn = PC->GetPawn();
	//		UHealthComponent* HealthComponent = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));

	//		if (ensure(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
	//		{
	//			return;
	//		}
	//	}
	//}

	//RestartDeadPlayers();
}

void AFFAGameMode::RestartDeadPlayers()
{
	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//{
	//	APlayerController* PC = It->Get();
	//	if (PC && PC->GetPawn() == nullptr)
	//	{
	//		RestartPlayer(PC);
	//	}
	//}
}
