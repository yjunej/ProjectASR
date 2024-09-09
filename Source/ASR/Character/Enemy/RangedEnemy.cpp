// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemy.h"

#include "Engine/SkeletalMeshSocket.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "ASR/Weapons/MeleeWeapon.h"
#include "ASR/Weapons/Projectile.h"

bool ARangedEnemy::ExecuteNormalAttack()
{
	bool bSuccess;
	bSuccess = Super::ExecuteNormalAttack();
	if (bSuccess)
	{
		AMeleeWeapon* RangedWeapon = GetMeleeWeapon();
		if (RangedWeapon != nullptr && WeaponAnimation != nullptr)
		{
			RangedWeapon->GetWeaponMesh()->PlayAnimation(WeaponAnimation, false);
			bool bSpawnSuccess = SpawnProjectile();
			return bSpawnSuccess;
		}
	}
	return false;
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
