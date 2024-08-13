// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASRDamageType.generated.h"

UENUM(BlueprintType)
enum class EASRDamageType : uint8
{
	EDT_Default						UMETA(DisplayName = "Default"),
	EDT_Die							UMETA(DisplayName = "Die"),

	EDT_FrontSmall					UMETA(DisplayName = "FrontSmall"),
	EDT_BackSmall					UMETA(DisplayName = "BackSmall"),
	EDT_LeftSmall					UMETA(DisplayName = "LeftSmall"),
	EDT_RightSmall					UMETA(DisplayName = "RightSmall"),

	EDT_FrontDown					UMETA(DisplayName = "FrontDown"),
	EDT_BackDown					UMETA(DisplayName = "BackDown"),

	EDT_FrontBig					UMETA(DisplayName = "FrontBig"),

	// Knock
	EDT_KnockDownBack				UMETA(DisplayName = "KnockDownBack"),
	EDT_KnockDownBackBig			UMETA(DisplayName = "KnockDownBackBig"),
	EDT_KnockDownBackSmash			UMETA(DisplayName = "KnockDownBackSmash"),

	EDT_KnockDownFront				UMETA(DisplayName = "KnockDownFront"),
	EDT_KnockDownFrontSpear			UMETA(DisplayName = "KnockDownFrontSpear"),
	EDT_KnockDownFrontSmashDouble	UMETA(DisplayName = "KnockDownFrontSmashDouble"),
	EDT_KnockDownFrontBig			UMETA(DisplayName = "KnockDownFrontBig"),

	// Aerial
	EDT_AerialStart					UMETA(DisplayName = "AerialStart"),
	EDT_AerialHit					UMETA(DisplayName = "AerialHit"),
	EDT_AerialKnockDown				UMETA(DisplayName = "AerialKnockDown"),

	EDT_MAX							UMETA(Hidden)
		
};