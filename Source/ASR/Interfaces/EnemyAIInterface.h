// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ASR/Enums/EnemyMovementSpeed.h"
#include "EnemyAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UEnemyAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASR_API IEnemyAIInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual class APatrolRoute* GetPatrolRoute() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed) = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetMovementSpeed() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetMoveToAttentionPoint(bool bEnable) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool GetMoveToAttentionPoint() const = 0;


	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const = 0;

	// TODO - AI Select Skills by State. Not By Input. Refactor combat system 
	UFUNCTION(BlueprintCallable)
	virtual bool AttackBegin(AActor* AttackTarget, int32 RequiredTokens) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void Attack(AActor* AttackTarget) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd(AActor* AttackTarget) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void StoreAttackTokens(AActor* AttackTarget, int32 Amount) = 0;

};
