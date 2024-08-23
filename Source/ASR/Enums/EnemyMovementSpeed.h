// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyMovementSpeed.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyMovementSpeed : uint8
{
	EMS_Idle					UMETA(DisplayName = "Idle"),
	EMS_Walk					UMETA(DisplayName = "Walk"),
	EMS_Jog						UMETA(DisplayName = "Jog"),
	EMS_Run						UMETA(DisplayName = "Run"),
	EMS_Sprint					UMETA(DisplayName = "Sprint"),
	EMS_MAX						UMETA(Hidden)
};
