// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRMainHUD.h"

#include "ASR/Character/ASRCharacter.h"
#include "Components/ProgressBar.h"

void UASRMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
	if (Owner != nullptr)
	{
		Owner->OnHealthChanged.AddDynamic(this, &UASRMainHUD::UpdateHealthBar);
	}
}

void UASRMainHUD::UpdateHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxHealth() > 0)
		{
			HealthBar->SetPercent(Owner->GetHealth() / Owner->GetMaxHealth());
		}

	}
}

void UASRMainHUD::UpdateStabilityBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxStability() > 0)
		{
			StabilityBar->SetPercent(Owner->GetStability() / Owner->GetMaxStability());
		}

	}
}
