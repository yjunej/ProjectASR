// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"

#include "ASR/Character/Gunner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


// Sets default values
ADrone::ADrone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ADrone::SelfDestruct, 30.f, false);
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (DroneOwner != nullptr)
    {
        // Follow the player
        FVector TargetLocation = DroneOwner->GetActorLocation() + FollowRelativePosition;
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, FollowSpeed));

		FVector2D ViewportSize;
		FVector2D CrosshairScreenPosition;
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		if (GEngine != nullptr && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}
		CrosshairScreenPosition.X = ViewportSize.X / 2.f;
		CrosshairScreenPosition.Y = ViewportSize.Y / 2.f;

		bool bSuccess = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairScreenPosition, CrosshairWorldPosition, CrosshairWorldDirection
		);

        // Aim at the target
        FRotator LookAtRotation = (CrosshairWorldPosition - GetActorLocation()).Rotation();
		LookAtRotation.Yaw += 90.f;
        SetActorRotation(LookAtRotation);

		FRotator OwnerRotation = DroneOwner->GetActorRotation();
		OwnerRotation.Yaw += 90.f;
		SetActorRotation(OwnerRotation);


        // Fire at the target
        //Fire(CrosshairWorldPosition);
    }

}

void ADrone::Fire(FVector FirePoint)
{
	const USkeletalMeshSocket* ProjectileSpawnSocket = MeshComponent->GetSocketByName(FName("Muzzle"));
	if (ProjectileSpawnSocket != nullptr)
	{

		FTransform SocketTransform = ProjectileSpawnSocket->GetSocketTransform(MeshComponent);
		FVector ToDestVector = FirePoint - SocketTransform.GetLocation();
		FRotator DestRotation = ToDestVector.Rotation();
		if (ProjectileClass != nullptr)
		{

			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Owner = this;
			ActorSpawnParameters.Instigator = DroneOwner;
			UWorld* World = GetWorld();
			if (World != nullptr)
			{

				AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
					ProjectileClass, SocketTransform.GetLocation(),
					DestRotation, ActorSpawnParameters
				);
				SpawnedProjectile->ProjectileOwner = this;
			}
		}
	}
}

void ADrone::SetDroneOwnerGunner(AGunner* Gunner)
{
	DroneOwner = Gunner;
}

void ADrone::SelfDestruct()
{
	// Sound, Effect
	DroneOwner->Drones.Remove(this);
	Destroy();
}

