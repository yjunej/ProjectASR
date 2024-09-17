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
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsFocusingTarget;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAttackTargetExists;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DesiredDirection;

	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess="true"))
	bool IsFocusingTarget() const;
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	bool IsAttackTargetExists() const;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void UpdateDesiredDirection();

	class IEnemyAIInterface* EnemyAIInterface;


};
