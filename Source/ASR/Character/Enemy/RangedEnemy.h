// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "RangedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ARangedEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ARangedEnemy();

protected:
	virtual void BeginPlay() override;

	virtual bool ExecuteNormalAttack(AActor* AttackTarget) override;
	virtual float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed) override;

	UFUNCTION(BlueprintCallable)
	bool SpawnProjectile();

private:
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivateAccess="true"))
	class UAnimationAsset* WeaponAnimation;

	UPROPERTY(EditAnywhere, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectile> ProjectileClass;

	

};
