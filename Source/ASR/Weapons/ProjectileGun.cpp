// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGun.h"

#include "Projectile.h"

#include "Engine/SkeletalMeshSocket.h"



void AProjectileGun::Fire(const FVector& Destination)
{
	Super::Fire(Destination);
	if (HasAuthority())
	{
		const USkeletalMeshSocket* ProjectileSpawnSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
		APawn* FireInstigator = Cast<APawn>(GetOwner());
		if (ProjectileSpawnSocket != nullptr && FireInstigator != nullptr)
		{
			FTransform SocketTransform = ProjectileSpawnSocket->GetSocketTransform(GetWeaponMesh());
			FVector ToDestVector = Destination - SocketTransform.GetLocation();
			FRotator DestRotation = ToDestVector.Rotation();
			if (ProjectileClass != nullptr)
			{
				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Owner = GetOwner();
				ActorSpawnParameters.Instigator = FireInstigator;
				UWorld* World = GetWorld();
				if (World != nullptr)
				{
					World->SpawnActor<AProjectile>(
						ProjectileClass, SocketTransform.GetLocation(),
						DestRotation, ActorSpawnParameters
					);
				}
			}
		}
	}

}
