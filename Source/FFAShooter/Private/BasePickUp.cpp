#include "BasePickUp.h"
#include "BasePowerUp.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

ABasePickUp::ABasePickUp()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	CoolDownDuration = 10.0f;
}

void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();

	Respawn();
}

void ABasePickUp::Respawn()
{
	if (PowerUpClass == nullptr)
	{
#if UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("PowerUp Class is nullptr in %s. Please update your Blueprint"), *GetName());
#endif
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerUpInstance = GetWorld()->SpawnActor<ABasePowerUp>(PowerUpClass, GetTransform(), SpawnParams);
}

void ABasePickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		//Set timer to re-spawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ABasePickUp::Respawn, CoolDownDuration);
	}
}

