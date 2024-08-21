// Fill out your copyright notice in the Description page of Project Settings.


#include "Slayer.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"


ASlayer::ASlayer()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	// Slower
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandGreatSwordSocket"));
}

void ASlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetMesh()->HideBoneByName(FName("shield_inner"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("sword_bottom"), EPhysBodyOp::PBO_Term);
	const USkeletalMeshSocket* RHKatanaSocket = GetMesh()->GetSocketByName(FName("RightHandKatanaSocket"));
}

void ASlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SlayerFirstSkillAction, ETriggerEvent::Triggered, this, &ASlayer::Input_FirstSkill);
	}
}

void ASlayer::FirstSkill()
{
	if (CanAttack())
	{
		ResetLightAttack();
		ResetHeavyAttack();
		bIsDodgePending = false;

		ExecuteFirstSkill();
	}
}

void ASlayer::LightAttack()
{
	if (CanAttack())
	{
		//if (GetVelocity().Size() >= 1000.f && LightAttackIndex == 0)
		//{
		//	DashLightAttack();
		//}
		//else
		//{
			//ResetHeavyAttack();
		ExecuteLightAttack(LightAttackIndex);
		//}
	}
	//else if (CanAttakInAir())
	//{
	//	ExecuteLightAttackInAir(LightAttackIndex);
	//}
}

void ASlayer::Input_FirstSkill(const FInputActionValue& Value)
{
	bIsLightAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsFirstSkillPending = true;
	}
	else
	{
		FirstSkill();
	}
}

void ASlayer::ResolveLightAttackPending()
{
	if (bIsFirstSkillPending)
	{
		bIsFirstSkillPending = false;
		bIsLightAttackPending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		FirstSkill();
	}
	// Only Resolve Light Attack in Parent function
	Super::ResolveLightAttackPending();
}

float ASlayer::GetFirstSkillWarpDistance() const
{
	return FirstSkillWarpDistance;
}

void ASlayer::ExecuteFirstSkill()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	PlayAnimMontage(FirstSkillMontage);
}
