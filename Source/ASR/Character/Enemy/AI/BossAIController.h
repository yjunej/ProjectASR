// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "BossAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABossAIController : public ABaseAIController
{
	GENERATED_BODY()

protected:
	virtual void SwitchToAttackState(AActor* TargetActor) override;

};
