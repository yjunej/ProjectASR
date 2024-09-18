// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ASR/Enums/EnemyMovementSpeed.h"
#include "ASR/Enums/EnemyAIState.h"
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
	virtual float GetCurrentHealth() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const = 0;


	// Attack Tokens System
	UFUNCTION(BlueprintCallable)
	virtual bool AIReserveAttackTokens(AActor* AttackTarget, int32 RequiredTokens) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void AIReturnAttackTokens(AActor* AttackTarget) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void StoreAttackTokens(AActor* AttackTarget, int32 Amount) = 0;

	// Integrated AI Attack method
	UFUNCTION(BlueprintCallable)
	virtual bool AIAttack(AActor* AttackTarget, EAIAttack AIAttackType) = 0;



};
