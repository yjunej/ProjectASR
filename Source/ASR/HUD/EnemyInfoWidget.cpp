// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInfoWidget.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Components/ProgressBar.h"

void UEnemyInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Owner != nullptr)
	{
		Owner->OnHealthChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
	}
}

void UEnemyInfoWidget::UpdateHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxHealth > 0)
		{
			HealthBar->SetPercent(Owner->Health / Owner->MaxHealth);
		}

	}
}
