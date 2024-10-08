// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAnimInstance.h"
#include "RootMotionEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API URootMotionEnemyAnimInstance : public UBaseEnemyAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AcceptanceRadius = 100.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsRootMotionActive = true;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DesiredDirection;


	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void UpdateDesiredDirection();

	class IEnemyAIInterface* EnemyAIInterface;

};
