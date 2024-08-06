// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangerWeapon.h"
#include "ProjectileGun.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API AProjectileGun : public ARangerWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& Destination) override;
	

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;


};
