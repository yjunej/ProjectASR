// Fill out your copyright notice in the Description page of Project Settings.


#include "Blader.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TargetingComponent.h"

ABlader::ABlader()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.f));

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
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ABlader::Input_Dodge);
	}
}

bool ABlader::CanAttack() const
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death && !GetCharacterMovement()->IsFalling())
	{
		return true;
	}
	return false;
}

bool ABlader::CanDodge() const
{
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
	ResetDodgeAttack();
	if (GetTargetingComponent() != nullptr)
	{
		GetTargetingComponent()->ClearSubTarget();
	}
	
}

void ABlader::Input_LightAttack(const FInputActionValue& Value)
{
	bIsHeavyAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
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
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsHeavyAttackPending = true;
	}
	else
	{
		HeavyAttack();
	}
}

void ABlader::Input_Dodge(const FInputActionValue& Value)
{
	if (CharacterState == EASRCharacterState::ECS_Attack || CharacterState == EASRCharacterState::ECS_Dodge)
	{
		bIsDodgePending = true;
	}
	else
	{
		Dodge();
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

void ABlader::Dodge()
{
	if (CanDodge())
	{
		SetCharacterState(EASRCharacterState::ECS_Dodge); 

		ResetLightAttack();
		ResetHeavyAttack();

		// Rotate Before Dodge
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		if (LastInputVector.Size() != 0.f)
		{
			SetActorRotation(UKismetMathLibrary::MakeRotFromX(LastInputVector));
		}


		GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("Dodge"), GetActorLocation() + GetActorForwardVector() * 150,
			GetActorRotation());
		PlayAnimMontage(DodgeMontage); 
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
			SetCharacterState(EASRCharacterState::ECS_Attack);

			// TODO: Dynamic Target Motion Warping 
			//GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
			//	FName("Forward"), GetActorLocation() + GetActorForwardVector() * LightAttackWarpDistance,
			//	GetActorRotation());

			PlayAnimMontage(LightAttackMontages[AttackIndex]);

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
			SetCharacterState(EASRCharacterState::ECS_Attack);

			// AnimNotify State handle this
			//GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
			//	FName("Forward"), GetActorLocation() + GetActorForwardVector() * HeavyAttackWarpDistance,
			//	GetActorRotation());

			PlayAnimMontage(HeavyAttackMontages[AttackIndex]);

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

void ABlader::ResolveDodgePending()
{
	if (bIsDodgePending)
	{
		bIsDodgePending = false;

		// Process Pending Dodge
		if (CharacterState == EASRCharacterState::ECS_Attack || CharacterState == EASRCharacterState::ECS_Dodge)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}

		// Try Heavy Attack (CanAttack check in this function)
		Dodge();

	}
}

