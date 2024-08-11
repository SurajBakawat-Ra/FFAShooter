// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAGameMode.h"
#include "HealthComponent.h"
#include "ShooterCharacter.h"
#include "SpawnPointActor.h"
#include "Kismet/GameplayStatics.h"


AFFAGameMode::AFFAGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxPoints = 5;
}

void AFFAGameMode::BeginPlay()
{
	ConfigureGameStart();
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

void AFFAGameMode::ConfigureGameStart()
{
	TArray<AActor*> FoundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPointActor::StaticClass(), FoundSpawnPoints);

	SpawnPoints.Empty();

	for (int i = 0; i < FoundSpawnPoints.Num(); i++)
	{
		SpawnPoints.Add(Cast<ASpawnPointActor>(FoundSpawnPoints[i]));
	}

	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), FoundCharacters);

	CharacterList.Empty();

	for (int i = 0; i < FoundCharacters.Num(); i++)
	{
		FString RandomName = "NONE";

		while (CheckCharacterWithNamesExist(RandomName) || RandomName.Equals("NONE"))
		{
			int32 RandomIndex = FMath::RandRange(0, CharacterNames.Num() - 1);
			RandomName = CharacterNames[RandomIndex];
		}

		CharacterList.Add(Cast<AShooterCharacter>(FoundCharacters[i]));

		CharacterList[i]->InGameName = RandomName;

		if (FoundSpawnPoints.Num() > 0)
		{
			int32 RandomSpawnIndex = FMath::RandRange(0, FoundSpawnPoints.Num() - 1);
			CharacterList[i]->SetActorLocation(FoundSpawnPoints[RandomSpawnIndex]->GetActorLocation());
			FoundSpawnPoints.RemoveAt(RandomSpawnIndex);
		}

		UE_LOG(LogTemp, Warning, TEXT("Character: %d, Name: %s"), CharacterList.Num(), *RandomName);
	}
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

TArray<AShooterCharacter*> AFFAGameMode::GetSortedCharacterList()
{
	// Copy the original CharacterList to a new array
	TArray<AShooterCharacter*> SortedCharacterList = CharacterList;

	// Sort the array by some property, e.g., Score
	SortedCharacterList.Sort([](const AShooterCharacter& A, const AShooterCharacter& B)
		{
			return A.CurrentPoints > B.CurrentPoints; // Sorting in descending order of score
		});

	return SortedCharacterList;
}

bool AFFAGameMode::CheckGameFinish(AShooterCharacter* ShooterCharacter)
{
	if (ShooterCharacter->CurrentPoints >= MaxPoints)
	{
		for (int i = 0; i < CharacterList.Num(); i++)
		{
			CharacterList[i]->SetAllowInput(false);
		}

		UE_LOG(LogTemp, Warning, TEXT("Winner: %s"), *ShooterCharacter->InGameName);

		OnCharacterWin.Broadcast(ShooterCharacter);

		return true;
	}

	return false;
}

void AFFAGameMode::SpawnAtRandomPoint(AShooterCharacter* ShooterCharacter)
{
	int32 RandomSpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	ShooterCharacter->SetActorLocation(SpawnPoints[RandomSpawnIndex]->GetActorLocation());
}
