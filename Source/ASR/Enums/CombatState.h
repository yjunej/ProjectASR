// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatState.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_None		UMETA(DisplayName = "Default State"),
	ECS_Attack		UMETA(DisplayName = "Attack"),
	ECS_Dodge		UMETA(DisplayName = "Dodge"),
	ECS_Guard		UMETA(DisplayName = "Guard"),
	ECS_Flinching	UMETA(DisplayName = "Flinching"),
	ECS_KnockDown	UMETA(DisplayName = "Knockout"),

	// Special State - Execution
	ECS_Death		UMETA(DisplayName = "Death"),

	ECS_MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EHitReactionState : uint8
{
	EHR_None		UMETA(DisplayName = "Default State"),
	EHR_Aerial		UMETA(DisplayName = "Aerial State"),
	EHR_Armor		UMETA(DisplayName = "Armor State"),
	EHR_SuperArmor  UMETA(DisplayName = "Super Armor State"),
	EHR_Parry		UMETA(DisplayName = "Parry State"),

	EHR_MAX			UMETA(Hidden)
};



