// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIPerceptionSense.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EAIPerceptionSense : uint8
{
	EPS_None				UMETA(DisplayName = "None"),
	EPS_Sight				UMETA(DisplayName = "Sight"),
	EPS_Hearing				UMETA(DisplayName = "Hearing"),
	EPS_Damage				UMETA(DisplayName = "Damage"),

	EPS_MAX					UMETA(Hidden),
};
