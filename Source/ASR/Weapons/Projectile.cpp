// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "ASR/Interfaces/HitInterface.h"
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

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherCOmp, FVector NormalImpulse, const FHitResult& HitResult)
{ 
	UE_LOG(LogTemp, Warning, TEXT("Projectile OnHit!"));

	IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
	if (HitInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile Hit!"));
		HitInterface->GetHit(HitResult, this, 75.f, EASRDamageType::EDT_FrontSmall);
		AASRCharacter* ASRCharacter = Cast<AASRCharacter>(ProjectileOwner);
		if (ASRCharacter != nullptr)
		{
			ASRCharacter->OnAttackEnemy();
			UGameplayStatics::PlaySoundAtLocation(this, HitEnemySound, ASRCharacter->GetActorLocation(), 1.f);
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
	if (HitObjectParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), HitObjectParticle, GetActorTransform()
		);
	}
	if (HitObjectSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitObjectSound, GetActorLocation());
	}
}

