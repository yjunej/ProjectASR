// Fill out your copyright notice in the Description page of Project Settings.


#include "Slayer.h"

#include "GameFramework/CharacterMovementComponent.h"

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