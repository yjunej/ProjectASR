// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemy.h"

#include "Engine/SkeletalMeshSocket.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "ASR/Weapons/MeleeWeapon.h"
#include "ASR/Weapons/Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"


ARangedEnemy::ARangedEnemy()
{
	bIsWeaponHidden = false;
	bIsCombatReady = true;
	AttackDistance = 1500.f;
	DefendDistance = 1500.f;
	SetAutoGuardRate(0.f); // Disable
}

void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();
}

bool ARangedEnemy::ExecuteNormalAttack(AActor* AttackTarget)
{
	bool bSuccess;
	bSuccess = Super::ExecuteNormalAttack(AttackTarget);
	if (bSuccess)
	{
		AMeleeWeapon* RangedWeapon = GetMeleeWeapon();
		if (RangedWeapon != nullptr && WeaponAnimation != nullptr)
		{
			RangedWeapon->GetWeaponMesh()->PlayAnimation(WeaponAnimation, false);
			return true;
		}
	}
	return false;
}

float ARangedEnemy::SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed)
{
	float NewSpeed;

	switch (EnemyMovementSpeed)
	{
	case EEnemyMovementSpeed::EMS_Idle:
		NewSpeed = 0.f;
		break;
	case EEnemyMovementSpeed::EMS_Walk:
		NewSpeed = 180.f;
		break;
	case EEnemyMovementSpeed::EMS_Jog:
		NewSpeed = 300.f;
		break;
	case EEnemyMovementSpeed::EMS_Run:
		NewSpeed = 500.f;
		break;
	case EEnemyMovementSpeed::EMS_Sprint:
		NewSpeed = 600.f;
		break;
	case EEnemyMovementSpeed::EMS_MAX:
	default:
		NewSpeed = 0.f;
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	return NewSpeed;
}

bool ARangedEnemy::SpawnProjectile()
{
	bool bSuccess = true;

	ABaseAIController* BaseAIController = Cast<ABaseAIController>(GetController());
	AActor* TargetActor = BaseAIController != nullptr ? BaseAIController->AttackTarget : nullptr;
	AMeleeWeapon* RangedWeapon = GetMeleeWeapon();


	if (TargetActor != nullptr && RangedWeapon != nullptr)
	{
		const USkeletalMeshSocket* ProjectileSpawnSocket = RangedWeapon->GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
		if (ProjectileSpawnSocket != nullptr)
		{
			FTransform SocketTransform = ProjectileSpawnSocket->GetSocketTransform(RangedWeapon->GetWeaponMesh());
			FVector ToDestVector = (TargetActor->GetActorLocation() - SocketTransform.GetLocation()).GetSafeNormal();
			FRotator DestRotation = ToDestVector.Rotation();
			if (ProjectileClass != nullptr)
			{

				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Owner = GetOwner();
				ActorSpawnParameters.Instigator = this;
				UWorld* World = GetWorld();
				if (World != nullptr)
				{
					AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
						ProjectileClass, SocketTransform.GetLocation(),
						DestRotation, ActorSpawnParameters
					);
					SpawnedProjectile->ProjectileOwner = this;
					return true;
				}
			}
		}
	}
	return false;
}
