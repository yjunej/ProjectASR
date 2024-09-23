// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyLockOnWidget.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Components/Image.h"

void UEnemyLockOnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LockOnMarker->SetVisibility(ESlateVisibility::Visible);
	ExecutionMarker->SetVisibility(ESlateVisibility::Collapsed);

	if (Owner != nullptr)
	{
		Owner->OnHealthChanged.AddDynamic(this, &UEnemyLockOnWidget::SelectMarker);
	}
}

void UEnemyLockOnWidget::SelectMarker()
{
	if (Owner != nullptr)
	{
		if (Owner->Health <= Owner->ExecutionThresholdHealth)
		{
			LockOnMarker->SetVisibility(ESlateVisibility::Collapsed);
			ExecutionMarker->SetVisibility(ESlateVisibility::Visible);
			PlayAnimation(CanExecute, 0.f, 0.f);
		}
		else
		{
			StopAnimation(CanExecute);
			ExecutionMarker->SetVisibility(ESlateVisibility::Collapsed);
			LockOnMarker->SetVisibility(ESlateVisibility::Visible);
		}
	}

}