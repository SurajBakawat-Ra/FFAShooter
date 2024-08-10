// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickUp.generated.h"

class ABasePowerUp;


UCLASS()
class FFASHOOTER_API ABasePickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePickUp();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<ABasePowerUp> PowerUpClass;

	ABasePowerUp* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CoolDownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	virtual void BeginPlay() override;

	void Respawn();

public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
