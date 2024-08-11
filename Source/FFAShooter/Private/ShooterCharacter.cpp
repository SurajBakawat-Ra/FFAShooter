// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "FFAShooter.h"
#include "HealthComponent.h"
#include "BaseWeapon.h"
#include "FFAGameMode.h"

#pragma region Init

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring"));
	CameraSpring->SetupAttachment(RootComponent);
	CameraSpring->bUsePawnControlRotation = true;
	CameraSpring->TargetArmLength = 160.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraSpring, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(FVector(0.f, 80.f, 0.f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);


	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponAttachSocketName = "WeaponSocket";
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FollowCamera->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &AShooterCharacter::OnHealthChanged);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector,
		FRotator::ZeroRotator, SpawnParams);

	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	CurrentStamina = MaxStamina;

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}

	InputAllowed = true;
}

#pragma endregion

#pragma region Input

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::StopFire);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AShooterCharacter::StopSprint);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::StartReload);


	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
}

void AShooterCharacter::MoveForward(float Val)
{
	if(InputAllowed)
		AddMovementInput(GetActorForwardVector() * Val);
}

void AShooterCharacter::MoveRight(float Val)
{
	if (InputAllowed)
		AddMovementInput(GetActorRightVector() * Val);
}

void AShooterCharacter::BeginCrouch()
{
	if (InputAllowed)
		Crouch();
}

void AShooterCharacter::EndCrouch()
{
	UnCrouch();
}

void AShooterCharacter::StartReload()
{
	if (CurrentWeapon && InputAllowed)
	{
		CurrentWeapon->StartReload();
	}
}

void AShooterCharacter::Sprint()
{
	if (CurrentStamina > 0 && !(GetCharacterMovement()->MovementMode == MOVE_None) && InputAllowed)
	{
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SprintSpeedMultiplier;
	}
}

void AShooterCharacter::StopSprint()
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShooterCharacter::BeginZoom()
{
	if(InputAllowed)
		bWantsToZoom = true;
}

void AShooterCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void AShooterCharacter::StartFire()
{
	if (CurrentWeapon && InputAllowed)
	{
		CurrentWeapon->StartFire();
	}
}

void AShooterCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

#pragma endregion

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckZoomAndStamina(DeltaTime);
}

void AShooterCharacter::CheckZoomAndStamina(float DeltaTimer)
{
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	if (IsSprinting && !GetCharacterMovement()->IsCrouching() && CurrentStamina > 0.f)
	{
		CurrentStamina -= DeltaTimer;
		TargetFOV *= 1.1f;
	}
	else if (CurrentStamina < MaxStamina)
	{
		StopSprint();
		CurrentStamina += DeltaTimer;
	}

	if (FollowCamera->FieldOfView != TargetFOV)
	{
		float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTimer, ZoomInterpSpeed);
		FollowCamera->SetFieldOfView(NewFOV);
	}
}


void AShooterCharacter::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetAllowInput(false);

		CurrentWeapon->Destroy();

		Cast<AShooterCharacter>(InstigatedBy->GetCharacter())->CurrentPoints++;

		OnDeath.Broadcast(this, Cast<AShooterCharacter>(InstigatedBy->GetCharacter()));

		if (!((AFFAGameMode*)GetWorld()->GetAuthGameMode())->CheckGameFinish(Cast<AShooterCharacter>(InstigatedBy->GetCharacter())))
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AShooterCharacter::SpawnPlayer, 5.0f, false);
		}
	}
}

void AShooterCharacter::SpawnPlayer()
{
	if (!((AFFAGameMode*)GetWorld()->GetAuthGameMode())->Winner)
	{
		bDied = false;

		OnSpawn.Broadcast(this);

		HealthComp->SetFullHealth();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		SetAllowInput(true);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}

		((AFFAGameMode*)GetWorld()->GetAuthGameMode())->SpawnAtRandomPoint(this);
	}
}

FVector AShooterCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		//Custom implementation, it returns the camera view.
		return FollowCamera->GetComponentLocation();
	}

	//If for some reason this fails, return the original character view location.
	return Super::GetPawnViewLocation();
}

void AShooterCharacter::SetAllowInput(bool Allow)
{
	InputAllowed = Allow;

	if (Allow)
	{
		GetCharacterMovement()->SetDefaultMovementMode();

		if (GetController()->IsPlayerController())
		{
			if (Cast<APlayerController>(GetController()))
			{
				EnableInput(Cast<APlayerController>(GetController()));
			}
		}
	}
	else
	{
		GetMovementComponent()->StopMovementImmediately();

		GetCharacterMovement()->DisableMovement();

		if (GetController()->IsPlayerController())
		{
			if (Cast<APlayerController>(GetController()))
			{
				DisableInput(Cast<APlayerController>(GetController()));
			}
		}
	}
}

