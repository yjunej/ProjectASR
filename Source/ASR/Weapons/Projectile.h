// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ASR_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	AActor* ProjectileOwner;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& HitResult);

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Trail;

	class UParticleSystemComponent* TrailComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* HitObjectEffect;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HitObjectParticle;

	UPROPERTY(EditAnywhere)
	class USoundCue* HitObjectSound;

	UPROPERTY(EditAnywhere)
	float ProjectileDamage;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* HitEnemyEffect;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HitEnemyParticle;

	UPROPERTY(EditAnywhere)
	class USoundCue* HitEnemySound;

public:	

};
