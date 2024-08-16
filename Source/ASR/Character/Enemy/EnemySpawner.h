// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ASR_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 300.f;

protected:
	virtual void BeginPlay() override;

private:
	void SpawnEnemy();

	UPROPERTY(VisibleAnywhere, Category = Collider, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	class AASRCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<class ABaseEnemy>, int32> EnemySpawnMap;

};
