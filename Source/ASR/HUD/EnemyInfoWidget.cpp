// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInfoWidget.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Components/ProgressBar.h"

void UEnemyInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyInfoWidget::UpdateHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxHealth > 0)
		{
			EnemyHealthBar->SetPercent(Owner->Health / Owner->MaxHealth);
		}

	}
}

void UEnemyInfoWidget::UpdateStaminaBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxStamina > 0)
		{
			EnemyStaminaBar->SetPercent(Owner->Stamina / Owner->MaxStamina);
		}
	}
}

void UEnemyInfoWidget::SetOwner(ABaseEnemy* NewOwner)
{
	if (Owner != NewOwner && NewOwner != nullptr)
	{	
		if (Owner != nullptr)
		{
			Owner->OnHealthChanged.RemoveDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
			Owner->OnStaminaChanged.RemoveDynamic(this, &UEnemyInfoWidget::UpdateStaminaBar);
		}
		Owner = NewOwner;
		Owner->OnHealthChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
		Owner->OnStaminaChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateStaminaBar);

		UpdateHealthBar();
		UpdateStaminaBar();
	}
}
