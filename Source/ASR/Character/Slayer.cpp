// Fill out your copyright notice in the Description page of Project Settings.


#include "Slayer.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"



ASlayer::ASlayer()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	// Slower
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandGreatSwordSocket"));

	ExecutionDistance = 700.f;
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
		EnhancedInputComponent->BindAction(SlayerSecondSkillAction, ETriggerEvent::Triggered, this, &ASlayer::Input_SecondSkill);
	}
}

void ASlayer::FirstSkill()
{
	if (CanAttack())
	{
		ResetNormalAttack();
		ResetDodge();
		ExecuteFirstSkill();
	}
}

void ASlayer::SecondSkill()
{
	if (CanAttack())
	{
		ResetNormalAttack();
		ResetDodge();
		ExecuteSecondSkill();
	}
}

void ASlayer::DashAttack()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetNormalAttack();
	ResetSkills();
	ResetDodge();
	PlayAnimMontage(DashAttackMontage);
}

void ASlayer::NormalAttack()
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

void ASlayer::ResetSkills()
{
	bIsFirstSkillPending = false;
	bIsSecondSkillPending = false;
}

void ASlayer::ResolveHeavyAttackPending()
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

void ASlayer::ResetState()
{
	Super::ResetState();
	ResetNormalAttack();
	ResetDodge();
	ResetSkills();
}

void ASlayer::Input_FirstSkill(const FInputActionValue& Value)
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

void ASlayer::Input_SecondSkill(const FInputActionValue& Value)
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

void ASlayer::ResolveLightAttackPending()
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

float ASlayer::GetFirstSkillWarpDistance() const
{
	return FirstSkillWarpDistance;
}

void ASlayer::ExecuteFirstSkill()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	PlayAnimMontage(FirstSkillMontage);
}

void ASlayer::ExecuteSecondSkill()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	PlayAnimMontage(SecondSkillMontage);
}

void ASlayer::SetExecutionCamera()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTargetWithBlend(GetExecutionCameraManager()->GetChildActor(), 0.2f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f, false);
}