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



UENUM(BlueprintType)
enum class EEnemyBehaviorState : uint8
{
	EBS_None						UMETA(DisplayName = "None"),
	EBS_Chase						UMETA(DisplayName = "Chase"),
	EBS_Strafe						UMETA(DisplayName = "Strafe"),
	EBS_Attack						UMETA(DisplayName = "Attack"),
	EBS_Disabled					UMETA(DisplayName = "Disabled"),
	EBS_Dead						UMETA(DisplayName = "Dead"),

	EBS_MAX							UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EAIAttack : uint8
{
	EAA_Default		UMETA(DisplayName = "Default Normal Attack"),
	EAA_ShortRange	UMETA(DisplayName = "Short Range Normal Attack"),
	EAA_LongRange	UMETA(DisplayName = "Long Range Normal Attack"),
	EAA_MAX			UMETA(Hidden),
};

