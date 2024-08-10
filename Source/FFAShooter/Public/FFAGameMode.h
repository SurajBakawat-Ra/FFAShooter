// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FFAGameMode.generated.h"

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

	virtual void Tick(float DeltaTime) override;

protected:

	void CheckAnyPlayerAlive();
	void RestartDeadPlayers();
	
};
