// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASR/Character/ASRAnimInstance.h"
#include "BaseEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API UBaseEnemyAnimInstance : public UASRAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsFocusingTarget;

	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess="true"))
	bool IsFocusingTarget() const;
	
};
