// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "Components/BoxComponent.h"
#include "ASR/Character/ASRCharacter.h"
#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnOverlapBegin);
}

void AEnemySpawner::SpawnEnemy()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawner NULL World"));
		return;
	}

	for (const TPair<TSubclassOf<ABaseEnemy>, int32>& Pair : EnemySpawnMap)
	{
		TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
		int32 NumberToSpawn = Pair.Value;

		for (int32 i = 0; i < NumberToSpawn; i++)
		{
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
				World,
				GetActorLocation(),
				RandomLocation,
				Radius
			);

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ABaseEnemy* SpawnedEnemy = World->SpawnActor<ABaseEnemy>(EnemyClass, RandomLocation, FRotator::ZeroRotator, ActorSpawnParams);
			if (SpawnedEnemy != nullptr && PlayerCharacter != nullptr)
			{
				FRotator LookAtRotator;
				LookAtRotator = SpawnedEnemy->GetActorRotation();
				LookAtRotator.Yaw = UKismetMathLibrary::FindLookAtRotation(SpawnedEnemy->GetActorLocation(), PlayerCharacter->GetActorLocation()).Yaw;
				SpawnedEnemy->SetActorRotation(LookAtRotator);
			}
		}
	}

}

void AEnemySpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AASRCharacter* OverlappedCharacter = Cast<AASRCharacter>(OtherActor);

	if (OverlappedCharacter != nullptr)
	{
		PlayerCharacter = OverlappedCharacter;

		//DrawDebugSphere(
		//	GetWorld(), PlayerCharacter->GetActorLocation(), 50.f, 6.f, FColor::Blue, false, 5.f);
		SpawnEnemy();
	}

}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

