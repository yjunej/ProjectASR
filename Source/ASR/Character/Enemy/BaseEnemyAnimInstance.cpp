// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimInstance.h"
#include "BaseAIController.h"

void UBaseEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateTargetingInfo();
}

void UBaseEnemyAnimInstance::UpdateTargetingInfo()
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		ABaseAIController* BaseAIController = Cast<ABaseAIController>(ControlPawn->GetController());
		if (BaseAIController != nullptr)
		{
			bIsFocusingTarget = BaseAIController->GetFocusActor() != nullptr;
			AttackTarget = BaseAIController->AttackTarget;
			bIsAttackTargetExists = AttackTarget != nullptr;
			return;
		}
	}
	bIsFocusingTarget = false;
	bIsAttackTargetExists = false;
	AttackTarget = nullptr;
}
