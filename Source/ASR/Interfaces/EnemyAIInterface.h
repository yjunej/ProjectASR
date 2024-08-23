// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ASR/Enums/EnemyMovementSpeed.h"
#include "EnemyAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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

	// Expose C++ Interface To Blueprint
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=AI)
	class APatrolRoute* GetPatrolRoute() const;
	virtual APatrolRoute* GetPatrolRoute_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AI)
	float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed);
	virtual float SetMovementSpeed_Implementation(EEnemyMovementSpeed EnemyMovementSpeed);

};
