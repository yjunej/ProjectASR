// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	void SetBlackboardKeys();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StrafeDistance = 800.f;

	UFUNCTION(BlueprintCallable)
	void ExecuteNormalAttack();

private:
	FTimerHandle TimerHandle;


};
