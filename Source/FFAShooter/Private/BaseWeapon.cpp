#include "BaseWeapon.h"
#include "FFAShooter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;
	HeadShotDamageMultiplier = 4.0f;
	BulletSpread = 2.0f;
	TimeBetweenShots = 0.1f;
	MagazineSize = 30;
	MaxAmmo = 90;
	ReloadTime = 1.25f;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	CurrentAmmo = MaxAmmo;
	CurrentMagAmmo = MagazineSize;
}

void ABaseWeapon::Fire()
{
	if (CurrentMagAmmo <= 0 || Reloading)
	{
		if (!Reloading)
		{
			//UE_LOG(LogTemp, Warning, TEXT("No ammo in magazine, starting reload"));
			StartReload();
		}
		//UE_LOG(LogTemp, Warning, TEXT("Already reloading"));
		return;
	}

	if (AActor* MyOwner = GetOwner())
	{
		FHitResult Hit;

		FVector  EyesLocation;
		FRotator EyesRotation;
		MyOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		FVector ShotDirection = EyesRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyesLocation + (ShotDirection * 10000.f);
		FVector TracerEndPoint = TraceEnd; //Particle target parameter.

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}

		PlayFireEffects(TracerEndPoint);

		HitScanTrace.TraceTo = TracerEndPoint;
		HitScanTrace.SurfaceType = SurfaceType;

		LastFiredTime = GetWorld()->TimeSeconds;
		CurrentMagAmmo--;

		if (SoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundCue, GetActorLocation());
		}
		//UE_LOG(LogTemp, Log, TEXT("Fired a shot. CurrentMagAmmo: %d, CurrentAmmo: %d"), CurrentMagAmmo, CurrentAmmo);
	}
}

void ABaseWeapon::StartFire()
{
	//Take the max value, the result or zero. Is some kind of way of clamping.
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ABaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ABaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ABaseWeapon::StartReload()
{
	if (CurrentAmmo <= 0 || CurrentMagAmmo == MagazineSize)
	{
		if (Reloading)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Already Reloading"));
			return;
		}

		//UE_LOG(LogTemp, Warning, TEXT("Cannot reload. CurrentAmmo: %d, CurrentMagAmmo: %d"), CurrentAmmo, CurrentMagAmmo);
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

	Reloading = true;

	Cast<AShooterCharacter>(GetOwner())->GetCharacterMovement()->DisableMovement();

	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ABaseWeapon::Reload, ReloadTime, false);
}

void ABaseWeapon::Reload()
{
	int32 AmmoNeeded = MagazineSize - CurrentMagAmmo;
	int32 AmmoToReload = FMath::Min(AmmoNeeded, CurrentAmmo);

	CurrentMagAmmo += AmmoToReload;
	CurrentAmmo -= AmmoToReload;

	Cast<AShooterCharacter>(GetOwner())->GetCharacterMovement()->SetDefaultMovementMode();

	Reloading = false;
	//UE_LOG(LogTemp, Log, TEXT("Reloaded. CurrentMagAmmo: %d, CurrentAmmo: %d"), CurrentMagAmmo, CurrentAmmo);
}

void ABaseWeapon::PlayFireEffects(const FVector& TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	if (APawn* MyOwner = Cast<APawn>(GetOwner()))
	{
		// Check if the Pawn's controller is a PlayerController
		if (APlayerController* PC = Cast<APlayerController>(MyOwner->GetController()))
		{
			// Get the PlayerCameraManager
			APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
			if (CameraManager && FireCamShake)
			{
				// Start the camera shake
				CameraManager->StartCameraShake(FireCamShake, 1.0f); // You can adjust the scale value as needed
			}
		}
	}
}

void ABaseWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;

	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;

		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}
