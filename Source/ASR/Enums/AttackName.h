// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackName.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EAttackName : uint8
{
	EAN_Default					UMETA(DisplayName = "Default"),
	
	EAN_L1						UMETA(DisplayName = "NormalAttack1"),
	EAN_L2						UMETA(DisplayName = "NormalAttack2"),
	EAN_L3						UMETA(DisplayName = "NormalAttack3"),
	EAN_L4						UMETA(DisplayName = "NormalAttack4"),
	EAN_L5						UMETA(DisplayName = "NormalAttack5"),

	EAN_H1						UMETA(DisplayName = "HeavyAttack1"),
	EAN_H2						UMETA(DisplayName = "HeavyAttack2"),
	EAN_H3						UMETA(DisplayName = "HeavyAttack3"),
	EAN_H4						UMETA(DisplayName = "HeavyAttack4"),
	EAN_H5						UMETA(DisplayName = "HeavyAttack5"),
	
	EAN_D1						UMETA(DisplayName = "DashAttack1"),
	EAN_D2						UMETA(DisplayName = "DashAttack2"),

	EAN_S1						UMETA(DisplayName = "Skill1"),
	EAN_S2						UMETA(DisplayName = "Skill2"),

	EAN_IA1						UMETA(DisplayName = "InAirAttack1"),
	EAN_IA2						UMETA(DisplayName = "InAirAttack2"),

	EAN_MAX						UMETA(Hidden)
};
