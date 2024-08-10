// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePowerUp.h"

ABasePowerUp::ABasePowerUp()
{
	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;

	bIsPowerUpActive = false;
}

void ABasePowerUp::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired();
		bIsPowerUpActive = false;
		OnPowerupStateChange(bIsPowerUpActive);

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}


void ABasePowerUp::ActivatePowerup(AActor* ActivateFor)
{
	OnActivated(ActivateFor);

	bIsPowerUpActive = true;
	OnPowerupStateChange(bIsPowerUpActive);

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ABasePowerUp::OnTickPowerUp, PowerupInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

