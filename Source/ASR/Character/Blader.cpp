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
	}
}

bool ABlader::CanAttack() const
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death)
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
}

void ABlader::Input_LightAttack(const FInputActionValue& Value)
{
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
		ExecuteLightAttack(LightAttackIndex);
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
			GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(FName("Forward"), GetActorLocation() + GetActorForwardVector() * LightAttackWarpDistance);
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

void ABlader::Input_HeavyAttack(const FInputActionValue& Value)
{
}

