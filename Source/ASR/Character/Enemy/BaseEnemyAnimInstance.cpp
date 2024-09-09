// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimInstance.h"
#include "AIController.h"

void UBaseEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	bIsFocusingTarget = IsFocusingTarget();
}

bool UBaseEnemyAnimInstance::IsFocusingTarget() const
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		AAIController* AIController = Cast<AAIController>(ControlPawn->GetController());
		if (AIController != nullptr)
		{
			return AIController->GetFocusActor() != nullptr;
		}
	}
	return false;
}
