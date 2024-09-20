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
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	}
	
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Record All Hit Actor
	UE_LOG(LogTemp, Warning, TEXT("Projectile Overlap: %s %s %s %s"), *GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *OtherActor->GetClass()->GetName());
	if (OtherActor == nullptr || OtherActor == this || OtherActor == ProjectileOwner)
	{
		return;
	}

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(OtherActor);
	bool bDestoryProjectile = true;
	if (CombatInterface != nullptr)
	{
		// [TODO] - Hit Data Handle Particle!
		FHitData HitData;
		HitData.Damage = ProjectileDamage;
		HitData.DamageType = EASRDamageType::EDT_FrontSmall;

		if (Cast<APawn>(OtherActor) != nullptr)
		{
			HitData.HitEffect = HitEnemyEffect;
			HitData.HitParticleEffect = HitEnemyParticle;
			HitData.HitSound = HitEnemySound;
			bDestoryProjectile = CombatInterface->GetHit(SweepResult, ProjectileOwner, HitData);

			AASRCharacter* ASRCharacter = Cast<AASRCharacter>(ProjectileOwner);
			if (ASRCharacter != nullptr)
			{
				// FPS Setting
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
				SweepResult.ImpactPoint,
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

	if (bDestoryProjectile)
	{
		Destroy();
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Same as OnBeginOverlap
	UE_LOG(LogTemp, Warning, TEXT("Projectile Hit: %s %s %s %s"), *GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *OtherActor->GetClass()->GetName());

	if (OtherActor == nullptr || OtherActor == this || OtherActor == ProjectileOwner)
	{
		return;
	}
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(OtherActor);
	bool bDestoryProjectile = true;
	if (CombatInterface != nullptr)
	{
		FHitData HitData;
		HitData.Damage = ProjectileDamage;
		HitData.DamageType = EASRDamageType::EDT_FrontSmall;

		if (Cast<APawn>(OtherActor) != nullptr)
		{
			HitData.HitEffect = HitEnemyEffect;
			HitData.HitParticleEffect = HitEnemyParticle;
			HitData.HitSound = HitEnemySound;
			bDestoryProjectile = CombatInterface->GetHit(Hit, ProjectileOwner, HitData);

			AASRCharacter* ASRCharacter = Cast<AASRCharacter>(ProjectileOwner);
			if (ASRCharacter != nullptr)
			{
				// FPS Setting
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
				Hit.ImpactPoint,
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

	if (bDestoryProjectile)
	{
		Destroy();
	}
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

