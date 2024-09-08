// Fill out your copyright notice in the Description page of Project Settings.


#include "RangerBattleComponent.h"

#include "ASR/Character/Ranger.h"
#include "ASR/Weapons/RangerWeapon.h"
#include "ASR/Player/ASRPlayerController.h"
#include "ASR/HUD/RangerHUD.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
URangerBattleComponent::URangerBattleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void URangerBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Ranger != nullptr)
	{
		Ranger->GetCharacterMovement()->MaxWalkSpeed = ArmedMaxWalkSpeed;
		 
		if (Ranger->GetFollowCamera())
		{
			DefaultFOV = Ranger->GetFollowCamera()->FieldOfView;
			FOV = DefaultFOV;
		}
	}
	// ...
	
}



// Called every frame
void URangerBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// TODO: Call Below function by Delegates for performance

	if (Ranger != nullptr && Ranger->IsLocallyControlled())
	{
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
		SetHUDCrosshair(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void URangerBattleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URangerBattleComponent, EquippedWeapon);
	DOREPLIFETIME(URangerBattleComponent, bIsAiming);

}


// ONLY PROCESS ON SERVER 
void URangerBattleComponent::EquipWeapon(ARangerWeapon* Weapon)
{
	if (Ranger != nullptr && Weapon != nullptr)
	{
		EquippedWeapon = Weapon;
		const USkeletalMeshSocket* RightHandWeaponSocket = Ranger->GetMesh()->GetSocketByName(FName("RightHandRifle"));
		if (RightHandWeaponSocket != nullptr)
		{
			RightHandWeaponSocket->AttachActor(EquippedWeapon, Ranger->GetMesh());
			
			EquippedWeapon->SetWeaponState(ERangerWeaponState::EWS_Equipped);
			EquippedWeapon->SetOwner(Ranger);
			EquippedWeapon->SetHUDAmmo();

			// Aiming same target while side direction moving, NOT Replicated! use On Rep
			Ranger->GetCharacterMovement()->bOrientRotationToMovement = false; 
			Ranger->bUseControllerRotationYaw = true;

		}
	}
}

void URangerBattleComponent::Fire(bool bFire)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bStartFire = bFire;

	

	if (bStartFire)
	{

		FireWithTimer();
	}
}

void URangerBattleComponent::FireWithTimer()
{
	if (CanFire())
	{

		bCanFire = false;
		ServerFire(HitPoint);
		StartFireTimer();
	}
}

void URangerBattleComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon != nullptr && Ranger != nullptr)
	{
		Ranger->GetCharacterMovement()->bOrientRotationToMovement = false;
		Ranger->bUseControllerRotationYaw = true;
	}
}

void URangerBattleComponent::SetAiming(bool IsAiming)
{
	bIsAiming = IsAiming;
	ServerSetAiming(IsAiming);
	if (Ranger != nullptr)
	{
		Ranger->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? ArmedMaxWalkSpeedAiming : ArmedMaxWalkSpeed;
	}
}

void URangerBattleComponent::TraceCrosshairs(FHitResult& HitResult)
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
		if (Ranger != nullptr)
		{
			float CameraToCharacter = (Ranger->GetActorLocation() - TraceStartPosition).Size();
			TraceStartPosition += CrosshairWorldDirection * (CameraToCharacter + 100.f);
		}
		FVector TraceEndPosition = TraceStartPosition + CrosshairWorldDirection * 50000.f;
		GetWorld()->LineTraceSingleByChannel(
			HitResult, TraceStartPosition, TraceEndPosition, ECollisionChannel::ECC_Visibility);
	}



}

void URangerBattleComponent::Reload()
{
	if (Ranger != nullptr)
	{
		if (Ranger->GetRangerBattleState() != ERangerBattleState::ERS_Reloading)
		{
			Ranger->SetRangerBattleState(ERangerBattleState::ERS_Reloading);
			Ranger->PlayReloadMontage();
		}
	}

}

void URangerBattleComponent::ReloadFinished()
{
	Ranger->SetRangerBattleState(ERangerBattleState::ERS_Normal);
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetAmmo(EquippedWeapon->GetMagazineCapacity());
		EquippedWeapon->SetHUDAmmo();
		
	}

	if (bStartFire)
	{
		FireWithTimer();
	}
}

void URangerBattleComponent::SetHUDCrosshair(float DeltaSeconds)
{
	if (Ranger == nullptr)
	{
		return;
	}
	if (PlayerController == nullptr)
	{
		PlayerController = Cast<AASRPlayerController>(Ranger->Controller);
	}
	if (PlayerController != nullptr)
	{
		URangerHUD* GunnerHUD = PlayerController->GunnerHUD;
		if (GunnerHUD != nullptr)
		{

			if (EquippedWeapon != nullptr)
			{
				GunnerHUD->SetCrosshairVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				GunnerHUD->SetCrosshairVisibility(ESlateVisibility::Hidden);
			}

			FVector2D JogSpeedRange(0.f, 600.f);
			FVector2D ClampRange(1.f, 2.f);
			FVector Velocity = Ranger->GetVelocity();
			Velocity.Z = 0.f;

			SpreadRate = FMath::GetMappedRangeValueClamped(JogSpeedRange, ClampRange, Velocity.Size());
			if (Ranger->bIsCrouched)
			{ 
				CrosshairCrouchCoef = FMath::FInterpTo(CrosshairCrouchCoef, 0.7, DeltaSeconds, 5.f);
			}
			else
			{
				CrosshairCrouchCoef = FMath::FInterpTo(CrosshairCrouchCoef, 1.f, DeltaSeconds, 10.f);
			}
			if (Ranger->GetCharacterMovement()->IsFalling())
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

			GunnerHUD->CrosshairSpreadRate = SpreadRate;
			GunnerHUD->SetCrosshair();
		}
	}
}

void URangerBattleComponent::InterpFOV(float DeltaSeconds)
{
	if (EquippedWeapon == nullptr) return;

	USpringArmComponent* CameraBoom = Ranger->GetCameraBoom();
	FVector TargetSocketOffsetAdditive = Ranger->bIsCrouched ? FVector(0.f, 0.f, CrouchArmHeightOffset) : FVector::ZeroVector;


	if (bIsAiming)
	{
		//FOV = FMath::FInterpTo(FOV, EquippedWeapon->GetZoomFOV(), DeltaSeconds, EquippedWeapon->GetZoomSpeed());
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ZoomArmLength, DeltaSeconds, 5.f);
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, ZoomArmSocketOffset + TargetSocketOffsetAdditive, DeltaSeconds, 5.f);

	}
	else
	{
		//FOV = FMath::FInterpTo(FOV, DefaultFOV, DeltaSeconds, ZoomSpeed);
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, Ranger->DefaultSpringArmLength, DeltaSeconds, 10.f);
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, Ranger->DefaultSpringArmSocketOffest + TargetSocketOffsetAdditive, DeltaSeconds, 10.f);

	}


	if (Ranger != nullptr && Ranger->GetFollowCamera() != nullptr)
	{
		Ranger->GetFollowCamera()->SetFieldOfView(FOV);
	}
}

void URangerBattleComponent::StartFireTimer()
{
	if (EquippedWeapon != nullptr && Ranger != nullptr)
	{
		Ranger->GetWorldTimerManager().SetTimer(FireTimer, this, &URangerBattleComponent::EndFireTimer, EquippedWeapon->GetFireDelay());
	}
}

void URangerBattleComponent::EndFireTimer()
{
	if (EquippedWeapon != nullptr)
	{
		bCanFire = true;
		if (EquippedWeapon->GetAmmo() <= 0)
		{
			Reload();
		}

		if (bStartFire && EquippedWeapon->IsAutomatic())
		{
			FireWithTimer();
		}
	}

}

bool URangerBattleComponent::CanFire()
{
	if (EquippedWeapon != nullptr && Ranger != nullptr)
	{
		return !EquippedWeapon->IsOutOfAmmo() && bCanFire && Ranger->GetRangerBattleState() == ERangerBattleState::ERS_Normal;
	}
	return false;
}

void URangerBattleComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Ranger != nullptr && EquippedWeapon != nullptr && Ranger->GetRangerBattleState() == ERangerBattleState::ERS_Normal)
	{


		Ranger->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		
		Ranger->AddControllerYawInput(0.5 * FMath::RandRange(-SpreadRate, SpreadRate));
		Ranger->AddControllerPitchInput(-0.2f + FMath::RandRange(-SpreadRate, SpreadRate));

	}
}

void URangerBattleComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void URangerBattleComponent::ServerSetAiming_Implementation(bool IsAiming)
{
	bIsAiming = IsAiming;
	if (Ranger != nullptr)
	{
		Ranger->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? ArmedMaxWalkSpeedAiming : ArmedMaxWalkSpeed;
	}
}
