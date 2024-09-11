// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "ASR/Interfaces/CombatInterface.h"
#include "ASR/Character/ASRCharacter.h"
#include "ASR/ASR.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true; // No Side Moving

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (Trail != nullptr)
	{
		TrailComponent = UGameplayStatics::SpawnEmitterAttached(
			Trail, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult)
{ 

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(OtherActor);
	if (CombatInterface != nullptr)
	{
		// [TODO] - Hit Data Handle Particle!
		//FHitData HitData = { .Damage = ProjectileDamage, .DamageType = EASRDamageType::EDT_FrontSmall, .HitEffect = HitEnemyParticle, .HitSound = HitEnemySound };
		FHitData HitData = { .Damage = ProjectileDamage, .DamageType = EASRDamageType::EDT_FrontSmall };

		if (Cast<APawn>(OtherActor) != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PROJECTILE HIT Pawn: %s"), *OtherActor->GetName());
			HitData.HitEffect = HitEnemyEffect;
			HitData.HitParticleEffect = HitEnemyParticle;
			HitData.HitSound = HitEnemySound;
			CombatInterface->GetHit(HitResult, ProjectileOwner, HitData);

			AASRCharacter* ASRCharacter = Cast<AASRCharacter>(ProjectileOwner);
			if (ASRCharacter != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitEnemySound, ASRCharacter->GetActorLocation(), 2.f);
				ASRCharacter->OnAttackEnemy();
			}
		}
	}
	else
	{
		if (HitObjectSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitObjectSound, GetActorLocation(), 2.f);
		}
		if (HitObjectEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitObjectEffect,
				HitResult.ImpactPoint,
				GetActorRotation(),
				FVector(1.f)
			);
		}
		else if (HitObjectParticle != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), HitObjectParticle, GetActorTransform()
			);
		}
	}


	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();
}

