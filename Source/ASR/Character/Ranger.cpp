// Fill out your copyright notice in the Description page of Project Settings.


#include "Ranger.h"

#include "ASR/ASR.h"
#include "ASR/Weapons/RangerWeapon.h"
#include "RangerBattleComponent.h"
#include "RangerAnimInstance.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"




// Sets default values
ARanger::ARanger()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Collision
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);



	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	
	// TPS Cam Setting
	CameraBoom->TargetArmLength = DefaultSpringArmLength;
	CameraBoom->SetRelativeLocation(DefaultSpringArmLocation);
	CameraBoom->SocketOffset = DefaultSpringArmSocketOffest;



	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CharacterInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
	CharacterInfoWidget->SetupAttachment(RootComponent);

	BattleComponent = CreateDefaultSubobject<URangerBattleComponent>(TEXT("BattleComponent"));
	BattleComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 900.f, 0.f);
	TurningInPlace = ETurningInPlace::ETIP_None;

}

// Called when the game starts or when spawned
void ARanger::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void ARanger::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARanger::Input_Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARanger::Input_Look);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ARanger::Input_Equip);
		EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &ARanger::Input_ToggleCrouch);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ARanger::Input_Reload);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARanger::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARanger::StopJumping);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ARanger::Input_Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ARanger::Input_StopAiming);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ARanger::Input_Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ARanger::Input_StopFiring);

		 
	}

}

void ARanger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARanger, OverlappedWeapon, COND_OwnerOnly);
}

void ARanger::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (BattleComponent != nullptr)
	{
		BattleComponent->Ranger = this;
	}
}

void ARanger::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}


void ARanger::Input_Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector ForwardDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		const FVector RightDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(ForwardDirectionVector, MoveVector.Y);
		AddMovementInput(RightDirectionVector, MoveVector.X);
	}
}

void ARanger::Input_Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ARanger::Input_Equip(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr && OverlappedWeapon != nullptr)
	{

		if (HasAuthority())
		{
			BattleComponent->EquipWeapon(OverlappedWeapon);
		}
		else
		{
			ServerEquip(Value) ;
		}
	}
}

void ARanger::Input_ToggleCrouch(const FInputActionValue& Value)
{
	if (GetMovementComponent()->IsFalling())
	{
		return;
	}

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
\
	}
}

void ARanger::Input_Aim(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr)
	{
		BattleComponent->SetAiming(true);
	}
}

void ARanger::Input_StopAiming(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr)
	{
		BattleComponent->SetAiming(false);
	}
}

void ARanger::Input_Fire(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr)
	{
		BattleComponent->Fire(true);
	}
}

void ARanger::Input_StopFiring(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr)
	{
		BattleComponent->Fire(false);
	}
}

void ARanger::Input_Reload(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr)
	{
		BattleComponent->Reload();
	}
}

void ARanger::TurnInPlace(float DeltaSeconds)
{
	if (AimOffsetYaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	else if (AimOffsetYaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	
	if (TurningInPlace != ETurningInPlace::ETIP_None)
	{
		InterpAimOffsetYaw = FMath::FInterpTo(InterpAimOffsetYaw, 0.f, DeltaSeconds, 5.f);
		AimOffsetYaw = InterpAimOffsetYaw;
		if (FMath::Abs(AimOffsetYaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_None;
			PrevAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // Record Rotation;
		}
	}
}

void ARanger::OnRep_OverlappedWeapon(ARangerWeapon* LastWeapon)
{
	if (OverlappedWeapon != nullptr)
	{
		OverlappedWeapon->SetInfoWidgetVisibility(true);
	}
	if (LastWeapon != nullptr)
	{ 
		LastWeapon->SetInfoWidgetVisibility(false);
	}
}

void ARanger::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}


void ARanger::ServerEquip_Implementation(const FInputActionValue& Value)
{
	if (BattleComponent != nullptr && OverlappedWeapon != nullptr)
	{
		BattleComponent->EquipWeapon(OverlappedWeapon);
	}
}


bool ARanger::IsWeaponEquipped()
{
	return (BattleComponent != nullptr && BattleComponent->EquippedWeapon != nullptr);
}

bool ARanger::IsAiming()
{
	return (BattleComponent != nullptr && BattleComponent->bIsAiming); 
}

void ARanger::SetOverlappedWeapon(ARangerWeapon* Weapon)
{

	if (OverlappedWeapon != nullptr)
	{
		OverlappedWeapon->SetInfoWidgetVisibility(false);
	}

	// Allow null pointer assign
	OverlappedWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappedWeapon != nullptr)
		{
			OverlappedWeapon->SetInfoWidgetVisibility(true);
		}
	}
}

void ARanger::UpdateAimOffset(float DeltaSeconds)
{
	if (BattleComponent == nullptr || BattleComponent->EquippedWeapon == nullptr)
	{
		return;
	}

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bInAir)
	{
		FRotator CurAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurAimRotation, PrevAimRotation);
		AimOffsetYaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_None)
		{
			InterpAimOffsetYaw = AimOffsetYaw;
		}

		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaSeconds);
	}
	else if (Speed > 0.f || bInAir)
	{
		PrevAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // Record Rotation;
		AimOffsetYaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_None;
		 
	}

	AimOffsetPitch = GetBaseAimRotation().Pitch;
	if (AimOffsetPitch > 90.f && !IsLocallyControlled())
	{
		// Correct Network Delivery Compression
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AimOffsetPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffsetPitch);
	}

}

ARangerWeapon* ARanger::GetEquippedWeapon() const
{
	if (BattleComponent != nullptr)
	{
		return BattleComponent->EquippedWeapon;
	}
	return nullptr;
}

void ARanger::PlayFireMontage(bool bAiming)
{
	FName SectionName = bAiming ? FName("RifleIronsights") : FName("RifleHip");
	if (BattleComponent != nullptr && BattleComponent->EquippedWeapon != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr && FireMontage != nullptr)
		{
			AnimInstance->Montage_Play(FireMontage);
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
	return;
}

void ARanger::PlayHitReactMontage()
{
	if (BattleComponent != nullptr && BattleComponent->EquippedWeapon != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr && HitReactMontage != nullptr)
		{
			AnimInstance->Montage_Play(HitReactMontage);
			FName SectionName("FromFront");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
	return;
}

void ARanger::PlayReloadMontage()
{
	FName SectionName("Rifle");
	if (BattleComponent != nullptr && BattleComponent->EquippedWeapon != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr && ReloadMontage != nullptr)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
	return;
}

void ARanger::HideCharacterClosedCamera()
{

	if (!IsLocallyControlled()) return;

	float DistanceToCamera = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size();


	bool bShouldHide = DistanceToCamera < CameraThreshold;


	GetMesh()->SetVisibility(!bShouldHide);

	if (BattleComponent != nullptr && BattleComponent->EquippedWeapon != nullptr)
	{
		BattleComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bShouldHide;
	}
}

FVector ARanger::GetHitPoint() const
{
	if (BattleComponent != nullptr)
	{
		return BattleComponent->HitPoint;
	}
	return FVector();
}


// Called every frame
void ARanger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO : Check Overhead
	UpdateAimOffset(DeltaSeconds);
	//HideCharacterClosedCamera();
}


