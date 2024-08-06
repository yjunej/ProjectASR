// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCase.generated.h"

UCLASS()
class ASR_API ABulletCase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletCase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherCOmp, FVector NormalImpulse,
		const FHitResult& HitResult);

private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* CaseMesh;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* CaseDropSoundCue;

	UPROPERTY(EditDefaultsOnly)
	float CaseEjectionImpulse = 10.f;



public:	
	
};
