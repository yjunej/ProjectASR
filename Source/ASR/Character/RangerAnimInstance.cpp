// Fill out your copyright notice in the Description page of Project Settings.


#include "RangerAnimInstance.h"


#include "Ranger.h"
#include "ASR/Weapons/RangerWeapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URangerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	RangerCharacter = Cast<ARanger>(GetOwningActor());
}

void URangerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (RangerCharacter == nullptr)
	{
		RangerCharacter = Cast<ARanger>(GetOwningActor());
		if (RangerCharacter == nullptr)
		{
			return;
		}
	}

	FVector Velocity = RangerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	UCharacterMovementComponent* CharacterMovement = RangerCharacter->GetCharacterMovement();
	if (CharacterMovement != nullptr)
	{
		bIsInAir = CharacterMovement->IsFalling();
		bIsAccelerating = CharacterMovement->GetCurrentAcceleration().Size() > 0.f ? true : false;
	}
	bIsWeaponEquipped = RangerCharacter->IsWeaponEquipped();
	EquippedWeapon = RangerCharacter->GetEquippedWeapon();

	bIsAiming = RangerCharacter->IsAiming();
	bIsCrouched = RangerCharacter->bIsCrouched;
	TurningInPlace = RangerCharacter->TurningInPlace; 

	FRotator AimRotation = RangerCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(RangerCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaAimRotation = FMath::RInterpTo(DeltaAimRotation, DeltaRot, DeltaSeconds, 15.f);
	AimMoveYawOffset = DeltaAimRotation.Yaw;


	FRotator CurrentRotation = RangerCharacter->GetActorRotation();
	FRotator DeltaCharacterRotation = (CurrentRotation - CharacterRotation).GetNormalized();

	float LeanToAngle = DeltaCharacterRotation.Yaw / DeltaSeconds;
	float RawLeanAngle = FMath::FInterpTo(LeanAngle, LeanToAngle, DeltaSeconds, 6.f);
	LeanAngle = FMath::Clamp(RawLeanAngle, -90.f, 90.f);

	AimOffsetYaw = RangerCharacter->AimOffsetYaw;
	AimOffsetPitch = RangerCharacter->AimOffsetPitch;

	// IK
	if (bIsWeaponEquipped && EquippedWeapon != nullptr &&
		EquippedWeapon->GetWeaponMesh() != nullptr && RangerCharacter->GetMesh() != nullptr)
	{
		LeftHandMatchingTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutVector;
		FRotator OutRotator;
		RangerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandMatchingTransform.GetLocation(),
			FRotator::ZeroRotator, OutVector, OutRotator);
		LeftHandMatchingTransform.SetLocation(OutVector);
		LeftHandMatchingTransform.SetRotation(FQuat(OutRotator));

		if (RangerCharacter->IsLocallyControlled())
		{

			bLocallyControlled = true;
			FTransform RightHandMactchingTransform = RangerCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			RightHandMatchingRotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, RightHandMactchingTransform.GetLocation() - RangerCharacter->GetHitPoint());

		}
	}

	bUseFABRIK = RangerCharacter->GetRangerBattleState() != ERangerBattleState::ERS_Reloading;
	bUseAimOffset = RangerCharacter->GetRangerBattleState() != ERangerBattleState::ERS_Reloading;
	bUseRightHandMatching = RangerCharacter->GetRangerBattleState() != ERangerBattleState::ERS_Reloading;
}
