// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletCase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABulletCase::ABulletCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CaseMesh"));
	SetRootComponent(CaseMesh);

	CaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	CaseMesh->SetSimulatePhysics(true);
	CaseMesh->SetEnableGravity(true);
	CaseMesh->SetNotifyRigidBodyCollision(true);

}

// Called when the game starts or when spawned
void ABulletCase::BeginPlay()
{
	Super::BeginPlay();
	CaseMesh->OnComponentHit.AddDynamic(this, &ABulletCase::OnHit); 
	CaseMesh->AddImpulse(GetActorRightVector() * CaseEjectionImpulse * FMath::RandRange(0.5f, 1.5f));
}

void ABulletCase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherCOmp, FVector NormalImpulse, const FHitResult& HitResult)
{
	if (CaseDropSoundCue != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CaseDropSoundCue, GetActorLocation());
	}
	Destroy();
}

// Called every frame
void ABulletCase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

