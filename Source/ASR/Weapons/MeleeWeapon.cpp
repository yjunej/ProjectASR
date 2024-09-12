// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ASR/Interfaces/CombatInterface.h"


AMeleeWeapon::AMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollison"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetupAttachment(RootComponent);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
	TraceStart->SetupAttachment(RootComponent);

	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
	TraceEnd->SetupAttachment(RootComponent);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnBoxBeginOverlap);
	
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeleeWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BeginOverlap!"));
	FHitResult WeaponBoxHitResult;
	WeaponBoxTrace(WeaponBoxHitResult);

	if (WeaponBoxHitResult.GetActor() != nullptr)
	{
		//UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		//ExecuteGetHit(BoxHit);
		//ICombatInterface* CombatInterface = Cast<ICombatInterface>(WeaponBoxHitResult.GetActor());
		//if (CombatInterface != nullptr)
		//{
		//	// Deprecated
		//	FHitData HitData = { .Damage = 100.f, .DamageType = EASRDamageType::EDT_FrontSmall };
		//	CombatInterface->GetHit(WeaponBoxHitResult, WeaponOwner, HitData);
		//}

	}
}

void AMeleeWeapon::WeaponBoxTrace(FHitResult& WeaponBoxHitResult)
{
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		TraceExtent,
		TraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		WeaponBoxHitResult,
		true
	);
	IgnoreActors.AddUnique(WeaponBoxHitResult.GetActor());
}
