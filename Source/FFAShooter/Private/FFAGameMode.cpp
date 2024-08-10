// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAGameMode.h"
#include "HealthComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"


AFFAGameMode::AFFAGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFFAGameMode::BeginPlay()
{
	ConfigureAllCharacters();
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

void AFFAGameMode::ConfigureAllCharacters()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), FoundActors);

	CharacterList.Empty();

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		FString RandomName = "NONE";

		while (CheckCharacterWithNamesExist(RandomName))
		{
			int32 RandomIndex = FMath::RandRange(0, CharacterNames.Num() - 1);
			RandomName = CharacterNames[RandomIndex];
		}

		CharacterList.Add(Cast<AShooterCharacter>(FoundActors[i]));

		CharacterList[i]->InGameName = RandomName;
	}

	UE_LOG(LogTemp, Warning, TEXT("Total Characters: %d"), CharacterList.Num());
}

bool AFFAGameMode::CheckCharacterWithNamesExist(FString Name)
{
	for (int i = 0; i < CharacterList.Num(); i++)
	{
		if (CharacterList[i]->InGameName.Equals(Name))
		{
			return true;
		}
	}

	return false;
}
