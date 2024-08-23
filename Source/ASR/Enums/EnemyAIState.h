// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIState.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	EAS_Passive				UMETA(DisplayName = "Passive"),
	EAS_Attack				UMETA(DisplayName = "Attack"),
	EAS_Frozen				UMETA(DisplayName = "Frozen"),
	EAS_Investigate			UMETA(DisplayName = "Investigate"),
	EAS_Dead				UMETA(DisplayName = "Dead"),
	EAS_MAX					UMETA(Hidden)
};
