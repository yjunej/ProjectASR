// Fill out your copyright notice in the Description page of Project Settings.


#include "Gunner.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ASR/Player/ASRPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "ASR/Weapons/BulletCase.h"
#include "ASR/Weapons/Projectile.h"



#include "ASR/HUD/RangerHUD.h"



AGunner::AGunner()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	// Slower
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandRifleSocket"));

	UltCameraChildComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("UltCamera"));
	UltCameraChildComponent->SetChildActorClass(ACameraActor::StaticClass());
	UltCameraChildComponent->SetupAttachment(RootComponent);
	UltCameraChildComponent->SetRelativeTransform(UltCameraTransform);

	ExecutionDistance = 700.f;

	PrimaryActorTick.bCanEverTick = true;

	// TPS Setting
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// TPS Cam Setting
	GetCameraBoom()->TargetArmLength = DefaultSpringArmLength;
	GetCameraBoom()->SetRelativeLocation(DefaultSpringArmLocation);
	GetCameraBoom()->SocketOffset = DefaultSpringArmSocketOffest;

	GetCameraBoom()->bUsePawnControlRotation = true;

}

void AGunner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FHitResult HitResult;
	TraceCrosshairs(HitResult);
	if (!HitResult.bBlockingHit)
	{
		HitPoint = HitResult.TraceEnd;
	}
	else
	{
		HitPoint = HitResult.ImpactPoint;
	}
	SetHUDCrosshair(DeltaSeconds);
	InterpFOV(DeltaSeconds);
}

void AGunner::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetMesh()->HideBoneByName(FName("weapon"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("pistol"), EPhysBodyOp::PBO_Term);
}

void AGunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SlayerFirstSkillAction, ETriggerEvent::Triggered, this, &AGunner::Input_FirstSkill);
		EnhancedInputComponent->BindAction(SlayerSecondSkillAction, ETriggerEvent::Triggered, this, &AGunner::Input_SecondSkill);
		EnhancedInputComponent->BindAction(SlayerUltAction, ETriggerEvent::Triggered, this, &AGunner::Input_Ult);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AGunner::Input_Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AGunner::Input_StopAiming);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AGunner::Input_Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AGunner::Input_StopFiring);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGunner::Input_Reload);
	}
}

void AGunner::FirstSkill()
{
	if (CanAttack())
	{
		ResetNormalAttack();
		ResetDodge();
		ExecuteFirstSkill();
	}
}

void AGunner::SecondSkill()
{
	if (CanAttack())
	{
		ResetNormalAttack();
		ResetDodge();
		ExecuteSecondSkill();
	}
}

void AGunner::DashAttack()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetNormalAttack();
	ResetSkills();
	ResetDodge();
	PlayAnimMontage(DashAttackMontage);
}

FVector AGunner::GetHitPoint() const
{
	return HitPoint;
}

void AGunner::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = ArmedMaxWalkSpeed;
	if (GetFollowCamera())
	{
		DefaultFOV = GetFollowCamera()->FieldOfView;
		FOV = DefaultFOV;
	}
}

void AGunner::NormalAttack()
{
	if (CanAttack())
	{
		if (GetVelocity().Size() >= 950.f && NormalAttackIndex == 0)
		{
			DashAttack();
		}
		else
		{
			ExecuteNormalAttack(NormalAttackIndex);
		}
	}
}

void AGunner::ResetSkills()
{
	bIsFirstSkillPending = false;
	bIsSecondSkillPending = false;
}

void AGunner::ResolveHeavyAttackPending()
{
	if (bIsSecondSkillPending)
	{
		bIsSecondSkillPending = false;
		bIsNormalAttackPending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		SecondSkill();
	}
}

void AGunner::ResetState()
{
	Super::ResetState();
	ResetNormalAttack();
	ResetDodge();
	ResetSkills();
}

void AGunner::Input_FirstSkill(const FInputActionValue& Value)
{
	bIsNormalAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsFirstSkillPending = true;
	}
	else
	{
		FirstSkill();
	}
}

void AGunner::Input_SecondSkill(const FInputActionValue& Value)
{
	bIsNormalAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsSecondSkillPending = true;
	}
	else
	{
		SecondSkill();
	}
}

void AGunner::Input_Ult(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		SetCharacterState(EASRCharacterState::ECS_Attack);
		PlayAnimMontage(UltMontage);
	}
}

void AGunner::Input_Fire(const FInputActionValue& Value)
{
	Fire(true);
}

void AGunner::Input_StopFiring(const FInputActionValue& Value)
{
	Fire(false);
}

void AGunner::Input_Aim(const FInputActionValue& Value)
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = ArmedMaxWalkSpeedAiming;
}

void AGunner::Input_StopAiming(const FInputActionValue& Value)
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = ArmedMaxWalkSpeed;
}

void AGunner::Input_Reload(const FInputActionValue& Value)
{
	Reload();
}

void AGunner::ResolveLightAttackPending()
{
	if (bIsFirstSkillPending)
	{
		bIsFirstSkillPending = false;
		bIsNormalAttackPending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		FirstSkill();
	}
	// Only Resolve Light Attack in Parent function
	Super::ResolveLightAttackPending();
}

float AGunner::GetFirstSkillWarpDistance() const
{
	return FirstSkillWarpDistance;
}

bool AGunner::IsAiming()
{
	return bIsAiming;
}

void AGunner::TraceCrosshairs(FHitResult& HitResult)
{
	FVector2D ViewportSize;
	FVector2D CrosshairScreenPosition;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	if (GEngine != nullptr && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	CrosshairScreenPosition.X = ViewportSize.X / 2.f;
	CrosshairScreenPosition.Y = ViewportSize.Y / 2.f;

	bool bSuccess = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairScreenPosition, CrosshairWorldPosition, CrosshairWorldDirection
	);

	if (bSuccess)
	{
		FVector TraceStartPosition = CrosshairWorldPosition;
		float CameraToCharacter = (GetActorLocation() - TraceStartPosition).Size();
		TraceStartPosition += CrosshairWorldDirection * (CameraToCharacter + 100.f);
		FVector TraceEndPosition = TraceStartPosition + CrosshairWorldDirection * 50000.f;
		GetWorld()->LineTraceSingleByChannel(
			HitResult, TraceStartPosition, TraceEndPosition, ECollisionChannel::ECC_Visibility);
	}
}

void AGunner::SetHUDCrosshair(float DeltaSeconds)
{
	if (GunnerPlayerController == nullptr)
	{
		GunnerPlayerController = Cast<AASRPlayerController>(Controller);
	}
	if (GunnerPlayerController != nullptr)
	{
		URangerHUD* RangerHUD = GunnerPlayerController->RangerHUD;
		if (RangerHUD != nullptr)
		{

			RangerHUD->SetCrosshairVisibility(ESlateVisibility::SelfHitTestInvisible);

			FVector2D JogSpeedRange(0.f, 600.f);
			FVector2D ClampRange(1.f, 2.f);
			FVector Velocity = GetVelocity();
			Velocity.Z = 0.f;

			SpreadRate = FMath::GetMappedRangeValueClamped(JogSpeedRange, ClampRange, Velocity.Size());
			if (bIsCrouched)
			{
				CrosshairCrouchCoef = FMath::FInterpTo(CrosshairCrouchCoef, 0.7, DeltaSeconds, 5.f);
			}
			else
			{
				CrosshairCrouchCoef = FMath::FInterpTo(CrosshairCrouchCoef, 1.f, DeltaSeconds, 10.f);
			}
			if (GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirCoef = FMath::FInterpTo(CrosshairInAirCoef, 2.f, DeltaSeconds, 5.f);
			}
			else
			{
				CrosshairInAirCoef = FMath::FInterpTo(CrosshairInAirCoef, 1.f, DeltaSeconds, 10.f);
			}
			if (bIsAiming)
			{
				CrosshairAimCoef = FMath::FInterpTo(CrosshairAimCoef, 0.7f, DeltaSeconds, 5.f);
			}
			else
			{
				CrosshairAimCoef = FMath::FInterpTo(CrosshairAimCoef, 1.f, DeltaSeconds, 10.f);
			}

			SpreadRate *= CrosshairCrouchCoef * CrosshairInAirCoef * CrosshairAimCoef;

			RangerHUD->CrosshairSpreadRate = SpreadRate;
			RangerHUD->SetCrosshair();
		}
	}
}

void AGunner::InterpFOV(float DeltaSeconds)
{
	USpringArmComponent* GunnerCameraBoom = GetCameraBoom();
	FVector TargetSocketOffsetAdditive = bIsCrouched ? FVector(0.f, 0.f, CrouchArmHeightOffset) : FVector::ZeroVector;


	if (bIsAiming)
	{
		//FOV = FMath::FInterpTo(FOV, EquippedWeapon->GetZoomFOV(), DeltaSeconds, EquippedWeapon->GetZoomSpeed());
		GunnerCameraBoom->TargetArmLength = FMath::FInterpTo(GunnerCameraBoom->TargetArmLength, ZoomArmLength, DeltaSeconds, 5.f);
		GunnerCameraBoom->SocketOffset = FMath::VInterpTo(GunnerCameraBoom->SocketOffset, ZoomArmSocketOffset + TargetSocketOffsetAdditive, DeltaSeconds, 5.f);

	}
	else
	{
		//FOV = FMath::FInterpTo(FOV, DefaultFOV, DeltaSeconds, ZoomSpeed);
		GunnerCameraBoom->TargetArmLength = FMath::FInterpTo(GunnerCameraBoom->TargetArmLength, DefaultSpringArmLength, DeltaSeconds, 10.f);
		GunnerCameraBoom->SocketOffset = FMath::VInterpTo(GunnerCameraBoom->SocketOffset, DefaultSpringArmSocketOffest + TargetSocketOffsetAdditive, DeltaSeconds, 10.f);

	}


	GetFollowCamera()->SetFieldOfView(FOV);
}

void AGunner::ExecuteFirstSkill()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	PlayAnimMontage(FirstSkillMontage);
}

void AGunner::ExecuteSecondSkill()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	PlayAnimMontage(SecondSkillMontage);
}

void AGunner::Fire(bool bFire)
{
	bStartFire = bFire;
	if (bStartFire)
	{
		FireWithTimer();
	}
}

void AGunner::FireWithTimer()
{
	if (CanFire())
	{
		bCanFire = false;

		PlayFireMontage(bIsAiming);
		WeaponFire(HitPoint);

		AddControllerYawInput(0.5 * FMath::RandRange(-SpreadRate, SpreadRate));
		AddControllerPitchInput(-0.2f + FMath::RandRange(-SpreadRate, SpreadRate));

		StartFireTimer();
	}
}

bool AGunner::CanFire()
{
	return bCanFire && Ammo > 0 && GunnerAnimState != EGunnerAnimState::EGA_Reloading;
}

void AGunner::PlayFireMontage(bool bAiming)
{
	FName SectionName = bAiming ? FName("RifleIronsights") : FName("RifleHip");
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && FireMontage != nullptr)
	{
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AGunner::StartFireTimer()
{
	GetWorldTimerManager().SetTimer(FireTimer, this, &AGunner::EndFireTimer, 0.1f);
}

void AGunner::EndFireTimer()
{
	bCanFire = true;
	if (Ammo <= 0)
	{
		Reload();
	}
	if (bStartFire)
	{
		FireWithTimer();
	}
}

void AGunner::Reload()
{
	if (GunnerAnimState != EGunnerAnimState::EGA_Reloading)
	{
		GunnerAnimState = EGunnerAnimState::EGA_Reloading;
		PlayReloadMontage();
	}
}

void AGunner::ReloadFinished()
{
	GunnerAnimState = EGunnerAnimState::EGA_Normal;
	Ammo = 30.f;
	GunnerPlayerController->SetRangerAmmo(Ammo);
	if (bStartFire)
	{
		FireWithTimer();
	}
}

void AGunner::PlayReloadMontage()
{
	FName SectionName("Rifle");
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && ReloadMontage != nullptr)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	return;
}

void AGunner::WeaponFire(const FVector& HitTargetPoint)
{
	if (WeaponFireAnimation != nullptr)
	{
		WeaponMeshComponent->PlayAnimation(WeaponFireAnimation, false);
	}
	if (CaseClass != nullptr)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMeshComponent->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket != nullptr)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMeshComponent);

			UWorld* World = GetWorld();
			FRotator SocketRotator = SocketTransform.GetRotation().Rotator();
			SocketRotator.Pitch += FMath::RandRange(-10.f, 10.f);
			SocketRotator.Yaw += FMath::RandRange(-10.f, 10.f);

			if (World != nullptr)
			{
				World->SpawnActor<ABulletCase>(
					CaseClass, SocketTransform.GetLocation(), SocketRotator);
			}
		}
	}
	Ammo = FMath::Clamp(Ammo - 1, 0, 40);
	GunnerPlayerController->SetRangerAmmo(Ammo);

	// Fire Projectile
	const USkeletalMeshSocket* ProjectileSpawnSocket = WeaponMeshComponent->GetSocketByName(FName("MuzzleFlash"));
	if (ProjectileSpawnSocket != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PROJECTILE SPAWN1"));

		FTransform SocketTransform = ProjectileSpawnSocket->GetSocketTransform(WeaponMeshComponent);
		FVector ToDestVector = HitTargetPoint - SocketTransform.GetLocation();
		FRotator DestRotation = ToDestVector.Rotation();
		if (ProjectileClass != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PROJECTILE SPAWN2"));

			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Owner = GetOwner();
			ActorSpawnParameters.Instigator = this;
			UWorld* World = GetWorld();
			if (World != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("PROJECTILE SPAWN3"));

				AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
					ProjectileClass, SocketTransform.GetLocation(),
					DestRotation, ActorSpawnParameters
				);
				SpawnedProjectile->ProjectileOwner = this;
			}
		}
	}

}

void AGunner::OnAttackEnemy()
{
	URangerHUD* RangerHUD = GunnerPlayerController->RangerHUD;
	RangerHUD->SetCrosshairColor(FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(CrosshairTimerHandle, this, &AGunner::ResetCrosshairColor, 0.5f, false);
}

void AGunner::ResetCrosshairColor()
{
	URangerHUD* RangerHUD = GunnerPlayerController->RangerHUD;
	RangerHUD->SetCrosshairColor(FLinearColor::Green);
}

void AGunner::SetUltCamera()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTargetWithBlend(UltCameraChildComponent->GetChildActor(), 0.2f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f, false);
}
