// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LegacyCameraShake.h"
#include "BaseWeapon.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

class USoundCue;

UCLASS()
class FFASHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ULegacyCameraShake> FireCamShake;

	UPROPERTY()
	FHitScanTrace HitScanTrace;

	FTimerHandle TimerHandle_TimeBetweenShots;
	FTimerHandle TimerHandle_ReloadTime;

	float LastFiredTime;

	virtual void Fire();

#pragma region DamageModifiers

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadShotDamageMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TimeBetweenShots;

	/*Bullet spread in degrees.*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;
#pragma endregion

#pragma region Effects

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* TracerEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundCue* SoundCue;

	void PlayFireEffects(const FVector& TracerEndPoint);
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

#pragma endregion

#pragma region AmmoAndReloading

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0))
	int32 MagazineSize;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0))
	int32 MaxAmmo;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	int32 CurrentMagAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool Reloading;

	void Reload();

#pragma endregion

public:

	void StartFire();
	void StopFire();
	void StartReload();

};
