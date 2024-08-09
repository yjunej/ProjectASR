// Fill out your copyright notice in the Description page of Project Settings.


#include "Blader.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionWarpingComponent.h"


ABlader::ABlader()
{
    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandKatanaSocket"));  // 소켓에 부착
}

void ABlader::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetMesh()->HideBoneByName(FName("weapon_l"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("weapon_r"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("grenade"), EPhysBodyOp::PBO_Term);

	const USkeletalMeshSocket* RHKatanaSocket = GetMesh()->GetSocketByName(FName("RightHandKatanaSocket"));


}

void ABlader::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &ABlader::Input_LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &ABlader::Input_HeavyAttack);

	}
}

bool ABlader::CanAttack() const
{
	// TODO: Handle Jump Attack
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death && !GetCharacterMovement()->IsFalling())
	{
		return true;
	}
	return false;
}



void ABlader::BeginPlay()
{
	Super::BeginPlay();
}

void ABlader::ResetState()
{
	Super::ResetState();
	ResetLightAttack();
	ResetHeavyAttack();
}

void ABlader::Input_LightAttack(const FInputActionValue& Value)
{
	bIsHeavyAttackPending = false;
	if (GetCharacterState() == EASRCharacterState::ECS_Attack)
	{
		bIsLightAttackPending = true;
	}
	else
	{
		LightAttack();
	}

}

void ABlader::LightAttack()
{
	if (CanAttack())
	{
		ResetHeavyAttack();
		ExecuteLightAttack(LightAttackIndex);
	}
}


void ABlader::Input_HeavyAttack(const FInputActionValue& Value)
{
	bIsLightAttackPending = false;
	if (GetCharacterState() == EASRCharacterState::ECS_Attack)
	{
		bIsHeavyAttackPending = true;
	}
	else
	{
		HeavyAttack();
	}
}



void ABlader::HeavyAttack()
{
	if (CanAttack())
	{
		// TODO: NOT RESET L/H Counter for custom combo
		ResetLightAttack();
		ExecuteHeavyAttack(HeavyAttackIndex);
	}
}

void ABlader::ExecuteLightAttack(int32 AttackIndex)
{
	if (AttackIndex >= LightAttackMontages.Num())
	{
		LightAttackIndex = 0;
	}
	else
	{
		if (LightAttackMontages.IsValidIndex(AttackIndex) && LightAttackMontages[AttackIndex] != nullptr)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			SetCharacterState(EASRCharacterState::ECS_Attack);
			GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("Forward"), GetActorLocation() + GetActorForwardVector() * LightAttackWarpDistance,
				GetActorRotation());
			AnimInstance->Montage_Play(LightAttackMontages[AttackIndex]);

			if (LightAttackIndex + 1 >= LightAttackMontages.Num())
			{
				LightAttackIndex = 0;
			}
			else
			{
				++LightAttackIndex;
			}
		}
	}
}

void ABlader::ResetLightAttack()
{
	bIsLightAttackPending = false;
	LightAttackIndex = 0;
}

void ABlader::ExecuteHeavyAttack(int32 AttackIndex)
{
	if (AttackIndex >= HeavyAttackMontages.Num())
	{
		HeavyAttackIndex = 0;
	}
	else
	{
		if (HeavyAttackMontages.IsValidIndex(AttackIndex) && HeavyAttackMontages[AttackIndex] != nullptr)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			SetCharacterState(EASRCharacterState::ECS_Attack);
			GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("Forward"), GetActorLocation() + GetActorForwardVector() * HeavyAttackWarpDistance,
				GetActorRotation());
			AnimInstance->Montage_Play(HeavyAttackMontages[AttackIndex]);

			if (HeavyAttackIndex + 1 >= HeavyAttackMontages.Num())
			{
				HeavyAttackIndex = 0;
			}
			else
			{
				++HeavyAttackIndex;
			}
		}
	}
}

void ABlader::ResetHeavyAttack()
{
	bIsHeavyAttackPending = false;
	HeavyAttackIndex = 0;
}

void ABlader::ResolveLightAttackPending()
{
	if (bIsLightAttackPending)
	{
		bIsLightAttackPending = false;

		// Process Pending L Attack
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}

		// Try Light Attack (CanAttack check in this function)
		LightAttack();

	}

}

void ABlader::ResolveHeavyAttackPending()
{
	if (bIsHeavyAttackPending)
	{
		bIsHeavyAttackPending = false;

		// Process Pending H Attack
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}

		// Try Heavy Attack (CanAttack check in this function)
		HeavyAttack();

	}
}
