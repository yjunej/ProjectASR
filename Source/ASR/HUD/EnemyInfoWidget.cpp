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
	UE_LOG(LogTemp, Warning, TEXT("UpdateHealthBar!"));
	if (Owner != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateHealthBar2!"));

		if (Owner->MaxHealth > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateHealthBar3!: %f"), EnemyHealthBar->GetPercent());
			EnemyHealthBar->SetPercent(Owner->Health / Owner->MaxHealth);
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
		}
		Owner = NewOwner;
		Owner->OnHealthChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
		UpdateHealthBar();
	}
}
