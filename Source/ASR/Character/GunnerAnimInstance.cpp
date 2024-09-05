// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerAnimInstance.h"
#include "Gunner.h"
#include "Kismet/KismetMathLibrary.h"


void UGunnerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GunnerCharacter = Cast<AGunner>(GetOwningActor());
}

void UGunnerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (GunnerCharacter == nullptr)
	{
		GunnerCharacter = Cast<AGunner>(GetOwningActor());
		if (GunnerCharacter == nullptr)
		{
			return;
		}
	}

	UCharacterMovementComponent* CharacterMovement = GunnerCharacter->GetCharacterMovement();
	bIsInAir = bIsFalling;
	bIsWeaponEquipped = true;

	bIsAiming = GunnerCharacter->IsAiming();
	bIsCrouched = GunnerCharacter->bIsCrouched;

	FRotator AimRotation = GunnerCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GunnerCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaAimRotation = FMath::RInterpTo(DeltaAimRotation, DeltaRot, DeltaSeconds, 15.f);
	AimMoveYawOffset = DeltaAimRotation.Yaw;


	FRotator CurrentRotation = GunnerCharacter->GetActorRotation();
	FRotator DeltaCharacterRotation = (CurrentRotation - CharacterRotation).GetNormalized();

	float LeanToAngle = DeltaCharacterRotation.Yaw / DeltaSeconds;
	float RawLeanAngle = FMath::FInterpTo(LeanAngle, LeanToAngle, DeltaSeconds, 6.f);
	LeanAngle = FMath::Clamp(RawLeanAngle, -90.f, 90.f);

	AimOffsetYaw = GunnerCharacter->AimOffsetYaw;
	AimOffsetPitch = GunnerCharacter->AimOffsetPitch;

	// IK
	if (GunnerCharacter->GetMesh() != nullptr)
	{
		LeftHandMatchingTransform = GunnerCharacter->WeaponMeshComponent->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutVector;
		FRotator OutRotator;
		GunnerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandMatchingTransform.GetLocation(),
			FRotator::ZeroRotator, OutVector, OutRotator);
		LeftHandMatchingTransform.SetLocation(OutVector);
		LeftHandMatchingTransform.SetRotation(FQuat(OutRotator));

		if (GunnerCharacter->IsLocallyControlled())
		{
			FTransform RightHandMactchingTransform = GunnerCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			RightHandMatchingRotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, RightHandMactchingTransform.GetLocation() - GunnerCharacter->GetHitPoint());
		}
	}
	bUseFABRIK = GunnerCharacter->GunnerAnimState != EGunnerAnimState::EGA_Reloading;
	bUseAimOffset = GunnerCharacter->GunnerAnimState != EGunnerAnimState::EGA_Reloading;
	bUseRightHandMatching = GunnerCharacter->GunnerAnimState != EGunnerAnimState::EGA_Reloading;
}
