// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDeath, AShooterCharacter*, KilledCharacter, AShooterCharacter*, Killer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawn, AShooterCharacter*, ShooterCharacter);

class ABaseWeapon;

UCLASS()
class FFASHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:

#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComp;

#pragma endregion

#pragma region CharacterOverrides

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion

#pragma region Input
	void MoveForward(float Val);
	void MoveRight(float Val);
	void BeginCrouch();
	void EndCrouch();
	void BeginZoom();
	void StartReload();
	void Sprint();
	void StopSprint();
	void EndZoom();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();
#pragma endregion

#pragma region Zoom
	bool bWantsToZoom;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void CheckZoomAndStamina(float DeltaTimer);
#pragma endregion

#pragma region Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ABaseWeapon> StarterWeaponClass;

	UPROPERTY(BlueprintReadOnly)
	ABaseWeapon* CurrentWeapon;
#pragma endregion

#pragma region StaminaSprinting

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float SprintSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly)
	float CurrentStamina;

	UPROPERTY()
	float WalkSpeed;

	UPROPERTY()
	float SprintInputEnabled;

	UPROPERTY(BlueprintReadOnly)
	bool IsSprinting;

#pragma endregion

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void SpawnPlayer();


public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterSpawn OnSpawn;

	virtual FVector GetPawnViewLocation() const override;

#pragma region GameModeVariables

	FString InGameName;

#pragma endregion

};
