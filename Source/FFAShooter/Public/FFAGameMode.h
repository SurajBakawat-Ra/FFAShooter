// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FFAGameMode.generated.h"


class AShooterCharacter;
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

	void ConfigureAllCharacters();
	bool CheckCharacterWithNamesExist(FString Name);

	TArray<FString> CharacterNames{ "John", "Kevin", "Ryan", "Michael", "David", "James", "Daniel", "Matthew", "Andrew", "Christopher", "Joshua", "Joseph", "Robert", "Brian", "William", "Thomas", "Anthony", "Jason", "Charles", "Steven", "Mark", "Paul", "Timothy", "Richard", "Eric", "George", "Scott", "Justin", "Stephen", "Alexander" };

public:

	UPROPERTY()
	TArray<AShooterCharacter*> CharacterList;
};
