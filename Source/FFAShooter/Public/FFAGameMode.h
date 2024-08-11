// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FFAGameMode.generated.h"

class AShooterCharacter;
class ASpawnPointActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterWin, AShooterCharacter*, ShooterCharacter);
/**
 * 
 */
UCLASS()
class FFASHOOTER_API AFFAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFFAGameMode();

private:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

	void CheckAnyPlayerAlive();
	void RestartDeadPlayers();
	void ConfigureGameStart();
	bool CheckCharacterWithNamesExist(FString Name);

	UPROPERTY(BlueprintReadOnly)
	int MaxPoints;

	UPROPERTY()
	TArray<ASpawnPointActor*> SpawnPoints;

	UPROPERTY()
	TArray<FString> CharacterNames{ "John", "Kevin", "Ryan", "Michael", "David", "James", "Daniel", "Matthew", "Andrew", "Christopher", "Joshua", "Joseph", "Robert", "Brian", "William", "Thomas", "Anthony", "Jason", "Charles", "Steven", "Mark", "Paul", "Timothy", "Richard", "Eric", "George", "Scott", "Justin", "Stephen", "Alexander" };
	
	UPROPERTY()
	TArray<AShooterCharacter*> CharacterList;

public:

	UFUNCTION(BlueprintCallable)
	TArray<AShooterCharacter*> GetSortedCharacterList();

	bool CheckGameFinish(AShooterCharacter* ShooterCharacter);

	void SpawnAtRandomPoint(AShooterCharacter* ShooterCharacter);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterWin OnCharacterWin;
};
